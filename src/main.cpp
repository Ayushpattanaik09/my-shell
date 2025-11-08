#include "utils.h"
#include <iostream>
#include <string>
#include <signal.h>
#include <sys/wait.h>

extern void sigchld_handler(int);
extern int run_pipeline(const Pipeline &pipeline);

int main() {
    struct sigaction sa{};
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sa, nullptr);

    std::string line;
    while (true) {
        std::cout << "myshell$ " << std::flush;
        if (!std::getline(std::cin, line)) break;
        if (line.empty()) continue;

        if (line == "exit" || line == "quit") break;
        if (line == "jobs") { print_jobs(); continue; }

        Pipeline p = parse_line(line);
        if (p.empty()) continue;

        if (p.size() == 1 && !p[0].argv.empty()) {
            auto &cmd = p[0];
            if (cmd.argv[0] == "fg" && cmd.argv.size()>=2) {
                std::string idstr = cmd.argv[1];
                if (idstr.size() && idstr[0]=='%') idstr = idstr.substr(1);
                int id = std::stoi(idstr);
                if (jobs.count(id)) {
                    pid_t pid = jobs[id].pid;
                    kill(pid, SIGCONT);
                    int status; waitpid(pid, &status, 0);
                    remove_job_by_pid(pid);
                } else std::cout<<"no such job\n";
                continue;
            }
            if (cmd.argv[0] == "bg" && cmd.argv.size()>=2) {
                std::string idstr = cmd.argv[1];
                if (idstr.size() && idstr[0]=='%') idstr = idstr.substr(1);
                int id = std::stoi(idstr);
                if (jobs.count(id)) {
                    pid_t pid = jobs[id].pid;
                    kill(pid, SIGCONT);
                    jobs[id].state = RUNNING;
                } else std::cout<<"no such job\n";
                continue;
            }
        }

        run_pipeline(p);
    }

    std::cout << "Goodbye!\n";
    return 0;
}
