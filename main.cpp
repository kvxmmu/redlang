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
    Semantic sem = get_semantic(tree);
    return 0;
}