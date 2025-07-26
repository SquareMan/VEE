//    Copyright 2025 Steven Casper
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.

#include "JobManager.hpp"

#include "Assert.hpp"
#include "Fibers.hpp"
#include "Logging.hpp"

#include <tracy/Tracy.hpp>

#include <algorithm>
#include <deque>
#include <mutex>
#include <ranges>

namespace vee {
extern void lock_thread_to_core(std::thread& thread, std::size_t core_num);

void job_main(void (*job)()) {
    job();
    JobManager::terminate();
}

void JobManager::worker_main() {
    ZoneScoped;
    tracy::SetThreadName("Worker");
    convert_thread_to_fiber(worker_fiber_);

    while (running) {
        if (!current_job_) {
            std::lock_guard lock(queue_mutex_);
            if (!fibers_.empty()) {
                current_job_ = fibers_.front();
                fibers_.pop_front();
            }
        }

        if (!current_job_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        log_trace("JobManager: Starting/Resuming {}", current_job_->fiber.name);
        switch_to_fiber(current_job_->fiber);
        if (post_scheduler_action) {
            switch (post_scheduler_action->type) {
            case PostSchedulerAction::Type::Yield: {
                std::lock_guard lock(queue_mutex_);
                fibers_.push_back(*current_job_);
                break;
            }
            case PostSchedulerAction::Type::Suspend: {
                auto& instance = JobManager::get();
                std::lock_guard lock(instance.wait_mutex);
                instance.waiting_jobs.emplace_back(*current_job_, post_scheduler_action->wait_counter);
                break;
            }
            case PostSchedulerAction::Type::Terminate: {
                // Kick jobs that are waiting on this one if we set the counter to 0
                if (current_job_->signal_counter && current_job_->signal_counter->fetch_sub(1) == 1) {
                    JobManager& instance = get();
                    std::lock_guard wait_lock(instance.wait_mutex);
                    for (auto wait_it = instance.waiting_jobs.begin();
                         wait_it != instance.waiting_jobs.end();
                         ++wait_it) {
                        if (wait_it->wait_counter == current_job_->signal_counter) {
                            log_trace(
                                "JobManager: Kicking {} due to completion of {}",
                                wait_it->job.fiber.name,
                                current_job_->fiber.name
                            );

                            std::lock_guard lock(instance.queue_mutex_);
                            instance.fibers_.emplace_back(wait_it->job);
                            wait_it = instance.waiting_jobs.erase(wait_it);
                            if (wait_it == instance.waiting_jobs.end()) {
                                break;
                            }
                        }
                    }
                }
                break;
            }
            }
            post_scheduler_action = std::nullopt;
        }
        current_job_ = std::nullopt;
    }

    convert_fiber_to_thread();
    destroy_fiber(worker_fiber_);
}


void JobManager::yield() {
    VASSERT(current_job_.has_value(), "Attempted to yield a job without a current job");
    post_scheduler_action.emplace(PostSchedulerAction::Type::Yield, nullptr);
    log_trace("JobManager: Yielding from {}", current_job_->fiber.name);
    switch_to_fiber(worker_fiber_);
}

void JobManager::terminate() {
    VASSERT(current_job_.has_value(), "Attempted to terminate a job without a current job");
    log_trace("JobManager: Terminating {}", current_job_->fiber.name);
    post_scheduler_action.emplace(PostSchedulerAction::Type::Terminate, nullptr);
    switch_to_fiber(worker_fiber_);
}

void JobManager::wait_for_counter(std::atomic<uint32_t>* counter) {
    VASSERT(current_job_.has_value(), "Attempted to suspend a job without a current job");
    for (int i = 0; i < 100; i++) {
        if (counter->load() == 0) {
            return;
        }
        _mm_pause();
    }

    log_trace("JobManager: Suspending {} on counter (0x{})", current_job_->fiber.name, static_cast<void*>(counter));
    post_scheduler_action.emplace(PostSchedulerAction::Type::Suspend, counter);
    switch_to_fiber(worker_fiber_);
}

void JobManager::init() {
    VASSERT(instance == nullptr, "JobManager was already initialized!");

    // TODO: Use custom allocators for engine system initialization
    instance = new JobManager();
}
void JobManager::shutdown() {
    VASSERT(instance != nullptr, "JobManager was already shutdown!");

    instance->running = false;

    for (auto& worker : instance->workers_) {
        worker.join();
    }

    delete instance;
}

JobManager& JobManager::get() {
    VASSERT(instance != nullptr, "JobManager must be initialized before it can be used.");

    return *instance;
}


JobManager::JobManager() {
    // ASSUMPTIONS: We're running on a hyper threaded CPU
    // TODO: Be more picky with which cores we use. For example: only use P cores on Intel; only use
    // cores on the same CCD on Ryzen; only use cores on the CCD with the 3D V-cache on Ryzen X3D
    unsigned int core_count = std::thread::hardware_concurrency() / 2;
    log_info("JobManager is creating {} worker threads.", core_count);

    workers_.reserve(core_count);
    for (unsigned int i = 0; i < core_count; ++i) {
        std::thread worker(&JobManager::worker_main, this);
        lock_thread_to_core(worker, i * 2);
        workers_.push_back(std::move(worker));
    }
}

JobManager::~JobManager() = default;

void JobManager::queue_job(JobDecl decl, std::atomic<uint32_t>* wait_counter) {
    Job job;
    job.signal_counter = decl.signal_counter;
    // TODO: Implement fiber pool and initialize job fibers in the scheduler for new jobs
    job.fiber = create_fiber(reinterpret_cast<void (*)()>(job_main), decl.name);
    job.fiber.context.arg = reinterpret_cast<uintptr_t>(decl.entry);

    if (job.signal_counter) {
        job.signal_counter->fetch_add(1);
    }

    if (wait_counter == nullptr || wait_counter->load() == 0) {
        std::lock_guard lock(queue_mutex_);
        fibers_.push_back(job);
    } else {
        std::lock_guard lock(wait_mutex);
        waiting_jobs.emplace_back(job, wait_counter);
    }
}
std::size_t JobManager::num_workers() const {
    return workers_.size();
}
} // namespace vee
