#include "utils.h"
#include <sstream>

static std::vector<std::string> split_tokens(const std::string &line) {
    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string tok;
    while (iss >> tok) tokens.push_back(tok);
    return tokens;
}

Pipeline parse_line(const std::string &line) {
    Pipeline pipeline;
    auto tokens = split_tokens(line);
    Command cur;
    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto &t = tokens[i];
        if (t == "|") {
            pipeline.push_back(cur);
            cur = Command();
        } else if (t == "<") {
            if (i + 1 < tokens.size()) cur.infile = tokens[++i];
        } else if (t == ">") {
            if (i + 1 < tokens.size()) { cur.outfile = tokens[++i]; cur.append = false; }
        } else if (t == ">>") {
            if (i + 1 < tokens.size()) { cur.outfile = tokens[++i]; cur.append = true; }
        } else if (t == "&") {
            cur.background = true;
        } else {
            cur.argv.push_back(t);
        }
    }
    if (!cur.argv.empty() || !cur.infile.empty() || !cur.outfile.empty() || cur.background)
        pipeline.push_back(cur);
    return pipeline;
}
