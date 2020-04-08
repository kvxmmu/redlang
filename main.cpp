#include <iostream>
#include <lang/parser/types.hpp>
#include <lang/parser/parser.hpp>

int main() {
    const std::string data = "func main(string[] args) {print(123);}"; // test text to test lexer
    auto lexed = lex(data);
    debug_print(lexed);
    return 0;
}
