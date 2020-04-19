#include <iostream>
#include <lang/parser/types.hpp>
#include <lang/parser/parser.hpp>
#include <fstream>
#include <lang/compiler/semantic.hpp>

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
    auto lexed = lex(data);
    auto tree = parse(lexed);
    std::cout << "IR Compiled: " << compile_semantic(tree).build() << std::endl; // own IR
    return 0;
}