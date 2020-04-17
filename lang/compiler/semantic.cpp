//
// Created by kvxmmu on 17.04.2020.
//

#include "semantic.hpp"

namespace SemanticSpace {
    static Pattern assignment = Pattern().add(ID).add(ASSIGNMENT).add("ANY").until_pattern(SEMICOLON);
    static Pattern define_variable = Pattern().add(ID).add(ID).add(ASSIGNMENT).add("ANY").until_pattern(SEMICOLON);
    static Pattern define_function = Pattern().add(FUNC).add(ID).add("$BRACKET").add("$BRACE");

    Semantizer top_level_semantizer = Semantizer().add("assign_variable",
                                             assignment).add("define_variable",
                                                     define_variable).add("define_function",
                                                             define_function);
}


Semantic get_semantic(AstTree &etree) {
    Semantic out = Semantic();
    RedIterator<std::vector<SimpleVariant*>, SimpleVariant*> it(etree.tree, nullptr);
    SemantizerPattern *ptr = SemanticSpace::top_level_semantizer.check(it).pattern;
    if (ptr == nullptr) {
        std::cout << "ptr = null" << std::endl;
    } else {
        std::cout << "ptr->name = " << ptr->name << std::endl;
    }
    return out;
}

token_type_t convert(const std::string &ref) {
    if (ref == "BRACKET")
        return BRACKET;
    else if (ref == "SBRACKET")
        return SBRACKET;
    else if (ref == "ANY")
        return NOTHING;
    return BRACE;
}


size_t Pattern::match(RedIterator<std::vector <SimpleVariant *>, SimpleVariant *>& it) {
    size_t pos = 0;
    while (!it.is_done(pos) && (pos < this->pattern.size())) {
        SimpleVariant *variant = it.peek(pos);
        std::string &var = this->pattern[pos];
        if (!this->check_pattern(var, variant))
            return 0;
        pos++;
    }
    if (!this->get_until.empty()) {
        while (!it.is_done(pos)) {
            SimpleVariant *variant = it.peek(pos);
            if (this->check_pattern(this->get_until, variant))
                return pos;
            pos++;
        }
    }
    return pos;
}

bool Pattern::check_pattern(std::string str, SimpleVariant *variant) {
    if (str == "ANY")
        return true;
    bool by_text = str.at(0) == '!';
    bool by_block = str.at(0) == '$';

    if (by_text || by_block) {
        str = str.substr(1);
    }
    if (by_text && variant->type == SIMPLE_OBJECT) {
        auto pToken = reinterpret_cast<Token*>(variant->memptr);
        return pToken->token == str;
    } else if (by_block && variant->type == TREE) {
        token_type_t tree_type = convert(str);
        if (tree_type == NOTHING)
            return true;
        auto pTree = reinterpret_cast<AstTree*>(variant->memptr);
        return tree_type == pTree->block_type;
    } else if (variant->type == SIMPLE_OBJECT) {
        auto pToken = reinterpret_cast<Token*>(variant->memptr);
        return pToken->type == std::stoi(str);
    }
    return false;
}
