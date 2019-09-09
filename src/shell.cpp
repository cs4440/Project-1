#include "../include/shell.h"

void Shell::run() {
    bool new_line = false;
    std::string line;
    std::vector<std::string> tokens;

    while(std::getline(std::cin, line)) {
        _parser.clear();
        _parser.set_string(line.c_str());
        new_line = _parser.parse();

        while(new_line) {
            std::cout << "> ";
            std::getline(std::cin, line);
            _parser.set_string(line.c_str());
            new_line = _parser.parse();
        }

        std::cout << "\nPrinting tokens:" << std::endl;
        tokens = _parser.get_tokens();
        for(const auto& t : tokens) std::cout << t << std::endl;

        std::cout << "size: " << tokens.size() << std::endl;
        std::cout << "do somethign with tokens here\n" << std::endl;

        if(!tokens.empty() && (tokens[0] == "quit()" || tokens[0] == "exit()"))
            break;
    }
}
