//
// Created by kvxmmu on 08.04.2020.
//

#ifndef REDLANG_PARSER_HPP
#define REDLANG_PARSER_HPP

#include <vector>
#include <lang/parser/types.hpp>
#include <iostream>

#define tokenizer_args RedIterator<std::string, char> &it, std::vector<Token> &tokens
#define auto_place it, tokens

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

// syntax constructions defined here
#define RIGHT_ARROW 1
#define LEFT_ARROW 2
#define SEMICOLON 3
#define IMPORT_STMT 4
#define FUNC_STMT 5
#define POINT 6



void debug_print(const std::vector<Token> &tokens);
bool any_match(const std::string &buff, const char &c);


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

std::vector<Token> lex(const std::string &data);

#endif //REDLANG_PARSER_HPP
