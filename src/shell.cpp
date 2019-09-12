#include "../include/shell.h"

// start the shell process
void Shell::run() {
    bool more_lines = false;
    std::string cmd, line;
    std::vector<Token> tokens;
    std::vector<std::vector<std::string>> args;
    std::queue<std::string> ops;

    while(cmd != "exit") {
        std::cout << "# ";
        std::getline(std::cin, line);

        // run parser on line
        _parser.clear();
        _parser.set_string(line.c_str());
        more_lines = _parser.parse();

        // parser will return more_lines = true if line ends in '\'
        while(more_lines) {
            std::cout << "> ";
            std::getline(std::cin, line);
            _parser.set_string(line.c_str());
            more_lines = _parser.parse();
        }

        tokens = _parser.get_tokens();

        if(!tokens.empty()) {
            cmd = tokens[0].string();

            if(cmd != "exit") {
                // split tokens into args and ops
                _parse_cmds_and_ops(tokens, args, ops);
                _run_cmds(args, ops);
                args.clear();
            }
        }
    }
}

// convert a vector of strings into commands and operators
// pre-condition: non-empty vector of strings are passed in
// post-condition: each command (w/ their arguments) is pushed in vvstr
//                 queue of operators are populated if there are ops
//                 tokens is unchanged
// @param tokens - input, vector of strings
// @param vvstr - ret ref, vector of vector of strings
// @param qops - ret ref, queue of operators
//
// Example:
// ls -al | cat | wc -l
//
// vvstr[0]: ls, -al
// vvstr[1]: cat
// vvstr[2]: wc, -l
//
// qops[0]: |
// qops[1]: |
void Shell::_parse_cmds_and_ops(const std::vector<Token> &tokens,
                                std::vector<std::vector<std::string>> &vvstr,
                                std::queue<std::string> &qops) {
    std::vector<std::string> args;

    for(std::size_t i = 0; i < tokens.size(); ++i) {
        if(tokens[i].type() == state_machine::STATE_OP) {
            vvstr.emplace_back(args);
            qops.push(tokens[i].string());
            args.clear();
        } else
            args.push_back(tokens[i].string());
    }
    vvstr.emplace_back(args);
}

// run shell commands with fork(), pipe(), exec()
// @param args - vector of vector of strings
// @param ops - queue of operators
//
// DETAILS
// parent will fork n number children commands
// children will use dup2 to connect previous file descriptors to current
//      file descriptors
bool Shell::_run_cmds(std::vector<std::vector<std::string>> &args,
                      std::queue<std::string> &ops) {
    std::string prev_op = "";
    std::string cur_op = "";
    char **cargs = nullptr;
    int *fd = nullptr, *cur_fd = nullptr, *prev_fd = nullptr;
    int pipe_status, wait_status = 0;
    pid_t pid, wait_pid;

    // create file descriptors for n operators
    fd = new int[2 * ops.size()];
    for(std::size_t i = 0; i < ops.size(); ++i) {
        pipe_status = pipe(fd + 2 * i);
        if(pipe_status < 0) {
            std::cerr << "pipe failed at pos " << i << std::endl;
            exit(1);
        }
    }

    for(std::size_t i = 0; i < args.size(); ++i) {
        // get current file descriptor
        cur_fd = fd + (2 * i);

        // get current argument vector
        cargs = _vec_str_to_char_args(args[i]);

        // get current operator
        if(ops.size()) cur_op = ops.front();

        pid = fork();

        if(pid < 0) {
            std::cerr << "fork failed" << std::endl;
            exit(1);
        } else if(pid == 0) {
            if(!prev_op.empty()) {
                if(prev_op == "|") {
                    dup2(prev_fd[0], STDIN_FILENO);  // stdout to pipe write
                    close(prev_fd[0]);               // close pipe read
                    close(prev_fd[1]);               // close pipe write
                }
            }

            if(!cur_op.empty()) {
                if(cur_op == "|") {
                    dup2(cur_fd[1], STDOUT_FILENO);  // stdout to pipe write
                    close(cur_fd[0]);                // close pipe read
                    close(cur_fd[1]);                // close pipe write
                }
            }
            execvp(cargs[0], cargs);
            std::cerr << "exec failed" << std::endl;
            exit(1);
        }

        if(prev_fd) {
            close(prev_fd[0]);
            close(prev_fd[1]);
        }

        prev_fd = cur_fd;
        prev_op = cur_op;
        cur_op = "";

        if(ops.size()) ops.pop();

        // deallocate cargs and all its children
        _deallocate_all(cargs, args[i].size());
    }

    delete[] fd;

    // continue to wait for all children to terminate
    while((wait_pid = wait(&wait_status)) > 0) {
    }

    return true;
}

// delete entire char** and its child elements
// @param arr - array of cstrings
// @param size - size of array
void Shell::_deallocate_all(char **arr, std::size_t size) {
    for(std::size_t i = 0; i < size; ++i) delete[] arr[i];
    delete[] arr;
}

// convert a vector of strings to char**
// return char** - arra of cstrings
// NOTE - caller must manually handle deallocations!
char **Shell::_vec_str_to_char_args(std::vector<std::string> &v_str) {
    char *cstr = nullptr;
    char **cargs = new char *[v_str.size() + 1];

    for(std::size_t i = 0; i < v_str.size(); ++i) {
        cstr = new char[v_str[i].size() + 1];
        std::copy(v_str[i].begin(), v_str[i].end(), cstr);
        cstr[v_str[i].size()] = '\0';
        cargs[i] = cstr;
    }
    cargs[v_str.size()] = nullptr;

    return cargs;
}
