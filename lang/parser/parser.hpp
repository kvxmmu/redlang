//
// Created by kvxmmu on 08.04.2020.
//

#ifndef REDLANG_PARSER_HPP
#define REDLANG_PARSER_HPP

#include <vector>
#include <lang/parser/types.hpp>
#include <iostream>

#define tokenizer_args RedIterator<std::string, char> &it, std::vector<Token> &tokens
#define parser_args RedIterator<std::vector<Token>, Token> &it, AstTree &tree
#define auto_place it, tokens
#define pauto(TYPE) it, *tree.new_tree(TYPE)

// tokens defined here
#define ID 1
#define STRING 2
#define OPEN_BRACKET 3
#define CLOSE_BRACKET 4
#define OPEN_BRACE 5
#define CLOSE_BRACE 6
#define INTEGER 7
#define FLOAT 8
#define OPERATOR 9
#define SYNTAX_CONSTRUCTION 10
#define SBRACKET_OPEN 11
#define SBRACKET_CLOSE 12
#define CHAR 13
#define INTERPOLATED_STRING 14
#define ONELINE_COMMENT 15
#define MULTILINE_COMMENT 16
#define SEMICOLON 17
#define COLON 18
#define ASSIGNMENT 19
#define COMMA 20

// operators
#define PLUS 21
#define MINUS 22
#define EQUALS 23
#define NOT_EQUALS 24
#define LESS_OR_EQUAL 25
#define GREATER_OR_EQUAL 26
#define LESS 27
#define GREATER 28
#define POW 29
#define MUL 30
#define DIV 31
#define MOD 32

// syntax constructions defined here
#define RIGHT_ARROW 33
#define LEFT_ARROW 34
#define IMPORT 35
#define FUNC 36
#define POINT 37


std::string repeat(const std::string &src, size_t count);


void debug_print(const std::vector<Token> &tokens);
void debug_print(AstTree &tree, size_t indent = 0);
bool any_match(const std::string &buff, const char &c);

inline void unexpected_token_error(Token &token) {
    std::cout << "RedLang ParseError: Unexpected token " << token.token << std::endl;
    exit(0);
}

inline void expecting_error(const std::string &expected) {
    std::cout << "RedLang ParseError: Expected token " << expected << std::endl;
    exit(0);
}


template <typename T, typename V>
class RedIterator {
public:
    size_t pos = 0;
    size_t length{};
    V default_value;

    const T& iterator;

    explicit RedIterator(const T& __iterator,
            const V& __default_value) : iterator(__iterator), default_value(__default_value) {
        this->length = __iterator.size();
    }

    void next(size_t step = 1) {
        this->pos += step;
    }

    bool is_done(size_t step = 0) {
        return (step + this->pos) >= this->length;
    }

    V peek(size_t npos = 0) {
        if (this->is_done(npos))
            return default_value;
        size_t index = this->pos + npos;
        return this->iterator[index];
    }

};


std::string peek_count(RedIterator<std::string, char> &it, size_t count);


// tokenizers
bool is_skip_character(tokenizer_args);
bool is_id(tokenizer_args);
bool is_digit(tokenizer_args);
bool is_operator(tokenizer_args);
bool is_block(tokenizer_args);
bool is_expr(tokenizer_args);
bool is_syntax_construction(tokenizer_args);
bool is_string(tokenizer_args);
bool is_character(tokenizer_args);
bool is_float(tokenizer_args);
bool is_oneline_comment(tokenizer_args);
bool is_comment(tokenizer_args);
bool is_assignment_or_semicolon(tokenizer_args);

// parsers
void parse_brace(parser_args);
void parse_brackets(parser_args);
void parse_sbrackets(parser_args);

std::vector<Token> lex(const std::string &data);
AstTree parse(const std::vector<Token> &tokens);

#endif //REDLANG_PARSER_HPP
