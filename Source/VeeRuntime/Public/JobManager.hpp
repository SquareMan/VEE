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

#pragma once
#include "Fibers.hpp"
#include "Name.hpp"


#include <deque>
#include <mutex>
#include <thread>
#include <vector>


namespace vee {

struct JobDecl {
    Name name;
    void (*entry)();
    std::atomic<uint32_t>* counter = nullptr;
};

struct Job {
    std::atomic<uint32_t>* counter = nullptr;
    Fiber fiber;
};

struct WaitingJob {
    Job job;
    std::atomic<uint32_t>* wait_counter = nullptr;
};

class JobManager {
public:
    static void init();
    static void shutdown();

    static JobManager& get();
    void queue_job(JobDecl decl);
    std::size_t num_workers() const;
    static void yield();
    static void terminate();
    static void wait_for_counter(std::atomic<uint32_t>* counter);

private:
    inline static JobManager* instance = nullptr;
    JobManager();

    void worker_main();

public:
    ~JobManager();
    JobManager(JobManager const&) = delete;
    JobManager& operator=(JobManager const&) = delete;
    JobManager(JobManager&&) = delete;
    JobManager& operator=(JobManager&&) = delete;

private:
    std::atomic<bool> running = true;
    std::vector<std::thread> workers_;

    std::mutex queue_mutex_;
    std::deque<Job> fibers_;

    std::mutex wait_mutex;
    std::vector<WaitingJob> waiting_jobs;

    thread_local inline static Fiber worker_fiber_;
    thread_local inline static std::optional<Job> current_job_;

    struct PostSchedulerAction {
        enum class Type { Yield, Suspend, Terminate };

        Type type;
        std::atomic<uint32_t>* counter;
    };
    thread_local inline static std::optional<PostSchedulerAction> post_scheduler_action;
};
} // namespace vee
