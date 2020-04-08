#include <iostream>
#include <lang/parser/types.hpp>
#include <lang/parser/parser.hpp>
#include <fstream>

std::string readfile(const std::string &name) {
    std::ifstream stream(name.c_str());
    std::string content;
    char c;

    while (stream.get(c)) {
        content += c;
    }
    stream.close();
    return content;
}

int main() {
    const std::string data = readfile("../test.rl"); // test text to test lexer
    std::cout << "ok" << std::endl;
    auto lexed = lex(data);
    debug_print(lexed);
    return 0;
}
