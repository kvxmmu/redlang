//
// Created by kvxmmu on 17.04.2020.
//

#ifndef REDLANG_SEMANTIC_HPP
#define REDLANG_SEMANTIC_HPP

#include <lang/parser/parser.hpp>
#include <lang/parser/types.hpp>
#include <utility>


class Pattern {
public:
    std::vector<std::string> pattern;
    std::string get_until = "";

    Pattern &add(const std::string &name) {
        pattern.push_back(name);
        return *this;
    }

    Pattern &add(const token_type_t &token_type) {
        pattern.push_back(std::to_string(token_type));
        return *this;
    }

    Pattern &until() {
        this->get_until = this->pattern[this->pattern.size() - 1];
        return *this;
    }

    Pattern &until_pattern(const std::string &str) {
        this->get_until = str;
        return *this;
    }

    Pattern &until_pattern(const token_type_t &token_type) {
        this->get_until = std::to_string(token_type);
        return *this;
    }

    size_t match(RedIterator<std::vector<SimpleVariant*>, SimpleVariant*> &it);
    bool check_pattern(std::string str, SimpleVariant *variant);
};

class SemantizerPattern {
public:
    std::string name;
    Pattern pattern;

    SemantizerPattern(std::string __name, Pattern __pattern) : name(std::move(__name)), pattern(std::move(__pattern)) {

    }
};

typedef struct {
    size_t result;
    SemantizerPattern *pattern;
} SemantizerResponse;

class Semantizer {
public:
    std::vector<SemantizerPattern> patterns;

    Semantizer &add(const std::string &name, const Pattern &pattern) {
        patterns.emplace_back(name, pattern);
        return *this;
    }

    SemantizerResponse check(RedIterator<std::vector<SimpleVariant*>, SimpleVariant*> &it) {
        for (SemantizerPattern &pattern : this->patterns) {
            size_t result = pattern.pattern.match(it);
            if (result != 0) {
                return SemantizerResponse{result, &pattern};
            }
        }

        return SemantizerResponse{0, nullptr};
    }
};


class Semantic {
public:
    int value = 1;
};

AstTree preparse(AstTree &tree);
Semantic get_semantic(AstTree &etree);



#endif //REDLANG_SEMANTIC_HPP
