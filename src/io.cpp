#include "utils.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <vector>
#include <iostream>
#include <cstring>

int run_pipeline(const Pipeline &pipeline) {
    if (pipeline.empty()) return 0;
    int n = pipeline.size();
    std::vector<int> pipes;
    if (n > 1) pipes.resize(2*(n-1));

    for (int i=0;i<n-1;i++) {
        if (pipe(&pipes[2*i]) < 0) { perror("pipe"); return -1; }
    }

    std::vector<pid_t> pids;
    for (int i=0;i<n;i++) {
        pid_t pid = fork();
        if (pid < 0) { perror("fork"); return -1; }
        if (pid == 0) {
            // child
            if (i > 0) dup2(pipes[2*(i-1)], STDIN_FILENO);
            if (i < n-1) dup2(pipes[2*i + 1], STDOUT_FILENO);

            if (!pipeline[i].infile.empty()) {
                int fd = open(pipeline[i].infile.c_str(), O_RDONLY);
                if (fd < 0) { perror("open infile"); _exit(1); }
                dup2(fd, STDIN_FILENO);
                close(fd);
            }
            if (!pipeline[i].outfile.empty()) {
                int flags = O_WRONLY | O_CREAT | (pipeline[i].append ? O_APPEND : O_TRUNC);
                int fd = open(pipeline[i].outfile.c_str(), flags, 0644);
                if (fd < 0) { perror("open outfile"); _exit(1); }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }

            for (size_t j=0;j<pipes.size();++j) close(pipes[j]);

            if (pipeline[i].argv.empty()) _exit(0);
            std::vector<char*> argv;
            for (auto &s : pipeline[i].argv) argv.push_back(const_cast<char*>(s.c_str()));
            argv.push_back(nullptr);
            execvp(argv[0], argv.data());
            perror("execvp");
            _exit(127);
        } else {
            pids.push_back(pid);
        }
    }

    for (size_t j=0;j<pipes.size();++j) close(pipes[j]);

    bool is_background = pipeline.back().background;
    if (is_background) {
        add_job(pids.back(), RUNNING, "<pipeline>");
        std::cout << "Started pipeline in background (pid " << pids.back() << ")\n";
        return 0;
    } else {
        for (pid_t pid : pids) {
            waitpid(pid, nullptr, 0);
        }
    }
    return 0;
}
