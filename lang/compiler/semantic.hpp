//
// Created by kvxmmu on 17.04.2020.
//

#ifndef REDLANG_SEMANTIC_HPP
#define REDLANG_SEMANTIC_HPP

#include <lang/parser/parser.hpp>
#include <lang/parser/types.hpp>
#include <utility>

#define callback_params RedIterator<std::vector<SimpleVariant*>, SimpleVariant*> &it
#define callback_auto it

#define VARIABLE_ASSIGNMENT 1

typedef RedIterator<std::vector<SimpleVariant*>, SimpleVariant*> DefaultIt;

namespace SemTypes {
    class FunctionDefine {
    public:
        std::string name;
        std::string return_type;

        FunctionDefine(std::string __name, std::string __return_type) : name(std::move(__name)),
            return_type(std::move(__return_type)) {

        }
    };

    class FunctionArgument {
    public:
        std::string type;
        std::string name;

        bool is_array;

        SimpleVariant *default_value = nullptr;

        FunctionArgument(std::string __type, std::string __name, bool _is_array) : type(std::move(__type)), name(std::move(__name)),
            is_array(_is_array){

        }

        FunctionArgument &set_default_value(SimpleVariant *__value) {
            this->default_value = __value;
            return *this;
        }
    };
}

class Pattern {
public:
    std::vector<std::string> pattern;
    std::string get_until = "";
    size_t (*custom_match)(RedIterator<std::vector<SimpleVariant*>, SimpleVariant*> &) = nullptr;

    Pattern &add(const std::string &name) {
        pattern.push_back(name);
        return *this;
    }

    Pattern &set_custom_match(size_t (*__custom_match)(RedIterator<std::vector<SimpleVariant*>, SimpleVariant*> &)) {
        this->custom_match = __custom_match;
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

void run_semantic(AstTree &etree, RedIterator<std::vector<SimpleVariant*>, SimpleVariant*> *common_iterator = nullptr);

// callbacks
void define_function(callback_params);
void define_variable(callback_params);

template <typename T, typename StructType, StructType first_value, void (*parse_function)(SemantizerResponse &, std::vector<T> &, DefaultIt &, StructType &)>
std::vector<T> parse_this(SimpleVariant *variant, Semantizer &sem) {
    std::vector<T> out;
    AstTree &tree = *reinterpret_cast<AstTree*>(variant->memptr);
    DefaultIt it(tree.tree, nullptr);
    StructType structure = first_value;
    while (!it.is_done()) {
        SemantizerResponse res = sem.check(it);
        if (res.pattern == nullptr) {
            std::cerr << "# Found piece of shit" << std::endl;
            exit(0);
        }

        parse_function(res, out, it, structure);
        it.next(res.result);
    }

    return out;
}

void parse_args(SemantizerResponse &res, std::vector<SemTypes::FunctionArgument> &args,
        DefaultIt &it, token_type_t &last);

#endif //REDLANG_SEMANTIC_HPP