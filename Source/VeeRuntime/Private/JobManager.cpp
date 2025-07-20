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
    tracy::SetThreadName("Worker");
    convert_thread_to_fiber(worker_fiber_);

    while (running) {
        {
            std::lock_guard lock(wait_mutex);
            for (auto job_it = waiting_jobs.begin(); job_it != waiting_jobs.end(); ++job_it) {
                VASSERT(
                    job_it->wait_counter != nullptr,
                    "Job {} is suspended but has no associated counter",
                    job_it->job.fiber.name.to_string()
                );
                if (job_it->wait_counter->load() == 0) {
                    current_job_ = job_it->job;
                    waiting_jobs.erase(job_it);
                    break;
                }
            }
        }


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

        switch_to_fiber(current_job_->fiber);
        if (current_job_) {
            std::lock_guard lock(queue_mutex_);
            fibers_.push_back(*current_job_);
        }
        current_job_ = std::nullopt;
    }

    convert_fiber_to_thread();
    destroy_fiber(worker_fiber_);
}


void JobManager::yield() {
    switch_to_fiber(worker_fiber_);
}

void JobManager::terminate() {
    VASSERT(current_job_.has_value(), "Attempted to terminate a job without a current job");
    if (current_job_->counter) {
        current_job_->counter->fetch_sub(1);
    }
    current_job_ = std::nullopt;
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


    {
        auto& instance = JobManager::get();
        std::lock_guard lock(instance.wait_mutex);
        instance.waiting_jobs.emplace_back(*current_job_, counter);
    }
    current_job_ = std::nullopt;
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

void JobManager::queue_job(JobDecl decl) {
    Job job;
    job.counter = decl.counter;
    job.fiber = create_fiber(reinterpret_cast<void (*)()>(job_main), decl.name);
    job.fiber.context.arg = reinterpret_cast<uintptr_t>(decl.entry);

    if (job.counter) {
        job.counter->fetch_add(1);
    }

    {
        std::lock_guard lock(queue_mutex_);
        fibers_.push_back(job);
    }
}
} // namespace vee
