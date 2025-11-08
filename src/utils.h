#pragma once
#include <string>
#include <vector>
#include <map>
#include <atomic>
#include <sys/types.h> // for pid_t

struct Command {
    std::vector<std::string> argv;
    bool background = false;
    std::string infile;
    std::string outfile;
    bool append = false;
};
using Pipeline = std::vector<Command>;

enum JobState { RUNNING, STOPPED, DONE };
struct Job {
    int id;
    pid_t pid;
    JobState state;
    std::string cmdline;
};

extern std::map<int, Job> jobs;
extern std::atomic<int> next_job_id;

void add_job(pid_t pid, JobState state, const std::string &cmdline);
void remove_job_by_pid(pid_t pid);
void print_jobs();
void sigchld_handler(int);
Pipeline parse_line(const std::string &line);
int run_pipeline(const Pipeline &pipeline);
