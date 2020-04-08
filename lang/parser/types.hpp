//
// Created by kvxmmu on 08.04.2020.
//

#ifndef REDLANG_TYPES_HPP
#define REDLANG_TYPES_HPP

#include <vector>
#include <cinttypes>
#include <string>

#define TREE 0
#define SIMPLE_OBJECT 1

#define BRACKET 0
#define BRACE 1
#define NOTHING 2


typedef uint8_t tree_type_t;
typedef uint16_t token_type_t;


struct SimpleVariant {
    void *memptr;
    tree_type_t type;
};

struct Token {
    std::string token;
    token_type_t type;
};


class AstTree {
public:
    std::vector<SimpleVariant*> tree;
    SimpleVariant *variant = nullptr;
    bool need_free = false;

    token_type_t block_type = NOTHING;

    AstTree() = default;

    AstTree(size_t alloc_size, SimpleVariant *__variant);

    AstTree* new_tree(token_type_t tkn_type = NOTHING);
    Token *new_token(const Token &tkn_ref);

    void clear_tree();

    inline void dealloc() {
        if (this->need_free) {
            delete this->variant;
            this->clear_tree();
            this->tree.clear();
            this->tree.shrink_to_fit();
        }
    }

    ~AstTree() {
        this->dealloc();
    }
};


void move_tree(const AstTree *ast, AstTree *dst);

SimpleVariant *create_variant(const AstTree* ast);
SimpleVariant *create_variant(const Token &tkn);


#endif //REDLANG_TYPES_HPP
