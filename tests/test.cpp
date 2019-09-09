#include <iostream>
#include <string>
#include "../include/parser.h"
#include "../include/shell.h"
#include "../include/tokenizer.h"

int main(int argc, char* argv[]) {
    Parser parser;
    std::vector<std::string> vec;
    std::string raw1 =
        R"(ls -al --path=="hello 'John Doe' world"\)";
    std::string raw2 =
        R"(ls>>pwds>as=='happy world')";
    std::string raw3 =
        R"(mv>\>sh\ )";

    std::string tokraw1 =
        R"(==path | || << >> <> <| |>\ --path=="Name1 'Name 2 Name3' Name4")";

    Tokenizer tk;
    Token token;

    std::cout << "CLI arguments\n";
    std::cout << "--------------------------" << std::endl;
    for(int i = 0; i < argc; ++i) std::cout << argv[i] << std::endl;
    std::cout << std::endl << std::endl;

    std::cout << "TESTING TOKENIZATION of\n" << tokraw1 << std::endl;
    std::cout << "-----------------------" << std::endl;
    tk.set_string(tokraw1.c_str());
    while(tk >> token) {
        std::cout << std::setw(10) << std::right << token.type_string() << ": "
                  << std::left << token.string() << std::endl;
    }

    std::cout << "\nTESTING PARSER" << std::endl;
    std::cout << "--------------" << std::endl;
    std::cout << "Parsing buffer 1:\n" << raw1 << std::endl;
    parser.set_string(raw1.c_str());
    parser.parse();

    std::cout << "\nResult:" << std::endl;
    vec = parser.get_tokens();
    for(auto& v : vec) std::cout << v << std::endl;

    std::cout << std::endl << std::endl;

    std::cout << "Parsing buffer 2:\n" << raw2 << std::endl;
    parser.clear();
    parser.set_string(raw2.c_str());
    parser.parse();

    std::cout << "\nResult:" << std::endl;
    vec = parser.get_tokens();
    for(auto& v : vec) std::cout << v << std::endl;

    std::cout << std::endl << std::endl;

    std::cout << "Parsing buffer 3:\n" << raw3 << std::endl;
    parser.clear();
    parser.set_string(raw3.c_str());
    parser.parse();

    std::cout << "\nResult:" << std::endl;
    vec = parser.get_tokens();
    for(auto& v : vec) std::cout << v << std::endl;

    /////
    Shell shell;

    std::cout << "running shell" << std::endl;
    shell.run();
    std::cout << "ending shell" << std::endl;

    return 0;
}
