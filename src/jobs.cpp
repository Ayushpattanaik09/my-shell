#include "utils.h"
#include <iostream>
#include <sys/wait.h>
#include <signal.h>

std::map<int, Job> jobs;
std::atomic<int> next_job_id{1};

void add_job(pid_t pid, JobState state, const std::string &cmdline) {
    int id = next_job_id++;
    jobs[id] = Job{id, pid, state, cmdline};
    std::cout << "["<<id<<"] " << pid << "\n";
}
void remove_job_by_pid(pid_t pid) {
    for (auto it = jobs.begin(); it != jobs.end(); ++it) {
        if (it->second.pid == pid) { jobs.erase(it); return; }
    }
}
void print_jobs() {
    for (auto &p : jobs) {
        std::cout << "["<<p.second.id<<"] "<<p.second.pid<<" ";
        if (p.second.state==RUNNING) std::cout<<"Running ";
        else if (p.second.state==STOPPED) std::cout<<"Stopped ";
        std::cout<<p.second.cmdline<<"\n";
    }
}

// SIGCHLD handler: reap finished background children
void sigchld_handler(int) {
    int status;
    pid_t pid;
    // Reap all finished children (non-blocking)
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        remove_job_by_pid(pid);
    }
}
