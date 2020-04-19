//
// Created by kvxmmu on 17.04.2020.
//

#include "semantic.hpp"

namespace Names {
    const static std::string assign_variable = "assign_variable";
    const static std::string define_variable = "define_variable";
    const static std::string define_function = "define_function";
    const static std::string create_lambda = "create_lambda";

    const static std::string ordered_arg = "ordered_arg";
    const static std::string default_arg = "default_arg";
    const static std::string array_arg = "array_arg";
    const static std::string comma = "comma";
    const static std::string call = "call";
    const static std::string load_attribute = "load_attribute";
    const static std::string trash = "trash";
    const static std::string call_attribute = "call_attribute";
    const static std::string load_id = "load_id";
    const static std::string load_index = "load_index";

    std::vector<token_type_t> operators = {
            ASSIGNMENT, PLUS, MINUS,
            POW, MUL, DIV, LESS,
            GREATER, GREATER_OR_EQUAL,
            MOD, RIGHT_ARROW, LEFT_ARROW,
            POINT
    };
}

namespace SemanticSpace {

    static Pattern assignment = Pattern().add(ID).add(ASSIGNMENT).add("ANY").until_pattern(SEMICOLON);
    static Pattern define_variable = Pattern().add(ID).add(ID).add(ASSIGNMENT).add("ANY").until_pattern(SEMICOLON);
    static Pattern define_function = Pattern().add(FUNC).add(ID).add("$BRACKET").add(RIGHT_ARROW).add(ID).add("$BRACE");
    static Pattern create_lambda = Pattern().add("$BRACKET").add(RIGHT_ARROW).add("$BRACE");
    static Pattern load_attribute = Pattern().add(POINT).add(ID);
    static Pattern trash = Pattern().add(ID).add(SEMICOLON);
    static Pattern load_id = Pattern().add(ID);

    size_t custom_call(DefaultIt &it) {
        size_t pos = 0;
        while (!it.is_done(pos)) {
            SimpleVariant *variant = it.peek(pos);
            if (variant->type == TREE) {
                auto pTree = reinterpret_cast<AstTree*>(variant->memptr);
                if (pTree->block_type == BRACKET)
                    return pos+1;
            } else {
                auto pToken = reinterpret_cast<Token*>(variant->memptr);
                for (token_type_t &tkn : Names::operators) {
                    if (pToken->type == tkn)
                        return 0;
                }
            }
            pos++;
        }

        return 0;
    }

    size_t custom_call_attribute(DefaultIt &it) {
        size_t match = 0;
        if ((match = load_attribute.match(it)) != 0) {
            SimpleVariant *nextval = it.peek(match);
            if (nextval != nullptr && nextval->type == TREE) {
                auto pTree = reinterpret_cast<AstTree*>(nextval->memptr);
                if (pTree->block_type == BRACKET)
                    return match + 1;
            }
        }
        return 0;
    }

    // function argument patterns
    static Pattern ordered_arg = Pattern().add(ID).add(ID);
    static Pattern default_arg = Pattern().add(ID).add(ID).add(ASSIGNMENT).add("_"+std::to_string(COMMA));
    static Pattern array_arg = Pattern().add(ID).add(ID).add("$SBRACKET");
    static Pattern comma = Pattern().add(COMMA);
    static Pattern call = Pattern().set_custom_match(custom_call);
    static Pattern call_attribute = Pattern().set_custom_match(custom_call_attribute);
    static Pattern load_index = Pattern().add("$SBRACKET");

    Semantizer top_level_semantizer = Semantizer()
            .add(Names::assign_variable,
                assignment)
            .add(Names::define_variable,
                 define_variable)
            .add(Names::define_function,
                 define_function)
            .add(Names::create_lambda,
                 create_lambda)
            .add(Names::trash, trash);

    Semantizer func_level_semantizer = Semantizer()
            .add(Names::assign_variable,
                    assignment)
            .add(Names::define_variable, define_variable)
            .add(Names::define_function, define_function)
            .add(Names::call, call)
            .add(Names::call_attribute, call_attribute)
            .add(Names::load_attribute, load_attribute)
            .add(Names::load_index, load_index)
            .add(Names::create_lambda, create_lambda)
            .add(Names::trash, trash)
            .add(Names::load_id, load_id);

    Semantizer arg_level_semantizer = Semantizer()
            .add(Names::default_arg, default_arg)
            .add(Names::array_arg, array_arg)
            .add(Names::ordered_arg, ordered_arg)
            .add(Names::comma, comma);

}


Codegen compile_semantic(AstTree &etree, RedIterator<std::vector<SimpleVariant*>, SimpleVariant*> *common_iterator) {
    RedIterator<std::vector<SimpleVariant*>, SimpleVariant*> *_it = nullptr;

    Codegen generator;

    if (common_iterator != nullptr) {
        _it = common_iterator;
    } else {
        RedIterator<std::vector<SimpleVariant*>, SimpleVariant*> i(etree.tree, nullptr);
        _it = &i;
    }
    RedIterator<std::vector<SimpleVariant*>, SimpleVariant*> &it = *_it;
    while (!it.is_done()) {
        SemantizerResponse res = SemanticSpace::func_level_semantizer.check(it);
        if (res.result == 0) {
            std::cerr << "!Found piece of crap" << std::endl;
            exit(1);
        }
        if (res.pattern->name == Names::define_function) {
            define_function(callback_auto);
        } else if (res.pattern->name == Names::define_variable) {
            define_variable(callback_auto);
        } else if (res.pattern->name == Names::assign_variable) {
            assign_variable(callback_auto);
        } else if (res.pattern->name == Names::call) {
            call(callback_auto);
        } else if (res.pattern->name == Names::load_attribute) {
            load_attribute(callback_auto);
        } else if (res.pattern->name == Names::load_id) {
            load_id(callback_auto);
        } else if (res.pattern->name == Names::call_attribute) {
            call_attribute(callback_auto);
        } else {
            std::cout << "Его оставили без внимания, а ты даже не знаешь его имени: " << res.pattern->name << std::endl;
        }
        it.next(res.result);
    }
    return generator;
}

void load_attribute(callback_params) {
    std::string attribute_name = reinterpret_cast<Token*>(it.peek(1)->memptr)->token;
    std::cout << "Load attribute " << attribute_name << std::endl;
}


void call(callback_params) {
    std::cout << "Call something" << std::endl;
}

void load_id(callback_params) {
    std::string id = reinterpret_cast<Token*>(it.peek(0)->memptr)->token;
    std::cout << "Load id " << id << std::endl;
}

void call_attribute(callback_params) {
    std::string attribute_name = reinterpret_cast<Token*>(it.peek(1)->memptr)->token;
    std::cout << "Call attribute " << attribute_name << std::endl;
}

void load_index(callback_params) {
    std::cout << "Load index(mne len schitatb)" << std::endl;
}

void define_function(callback_params) {
    std::string name = reinterpret_cast<Token*>(it.peek(1)->memptr)->token;
    SimpleVariant *args = it.peek(2);
    auto arguments = parse_this<SemTypes::FunctionArgument, token_type_t, 1, parse_args>(args, SemanticSpace::arg_level_semantizer);
    std::string return_type = reinterpret_cast<Token*>(it.peek(4)->memptr)->token;
    auto block = reinterpret_cast<AstTree*>(it.peek(5)->memptr);
    SemTypes::FunctionDefine function(name, return_type, arguments, block);
    generator.submit_function_create(function);
}

void define_variable(callback_params) {
    std::string type = reinterpret_cast<Token*>(it.peek(0)->memptr)->token;
    std::string id = reinterpret_cast<Token*>(it.peek(1)->memptr)->token;

    std::vector<SimpleVariant*> expression;

    size_t pos = 3;
    while (!it.is_done(pos)) {
        SimpleVariant *variant = it.peek(pos);
        if (variant->type == SIMPLE_OBJECT) {
            auto tkn = reinterpret_cast<Token*>(variant->memptr);
            if (tkn->type == SEMICOLON)
                break;
        }
        expression.push_back(variant);
        pos++;
    }
    SemTypes::VariableDefine define(type, id, expression);
    generator.submit_variable_define(define);

}

void assign_variable(callback_params) {
    std::string id = reinterpret_cast<Token*>(it.peek(0)->memptr)->token;

    std::vector<SimpleVariant*> expression;

    size_t pos = 2;
    while (!it.is_done(pos)) {
        SimpleVariant *variant = it.peek(pos);
        if (variant->type == SIMPLE_OBJECT) {
            auto tkn = reinterpret_cast<Token*>(variant->memptr);
            if (tkn->type == SEMICOLON)
                break;
        }
        expression.push_back(variant);
        pos++;
    }
    SemTypes::VariableAssign vassign(id, expression);
    generator.submit_variable_assign(vassign);
}

void except(const std::string &comment) {
    std::cout << "RedLang SemanticError: " << comment << std::endl;
    exit(0);
}

void parse_args(SemantizerResponse &res, std::vector<SemTypes::FunctionArgument> &args,
        DefaultIt &it, token_type_t &last) {
    std::string name = res.pattern->name;
    if (name == Names::comma) {
        if (last == COMMA)
            except("Params must be comma-separated(not ,,)");
        last = COMMA;
        return;
    }
    if (last != COMMA && last != 1)
        except("Params must be comma-separated");
    std::string type = reinterpret_cast<Token*>(it.peek(0)->memptr)->token;
    std::string id = reinterpret_cast<Token*>(it.peek(1)->memptr)->token;
    if (name == Names::ordered_arg || name == Names::array_arg) {
        SemTypes::FunctionArgument arg(type, id, name == Names::array_arg);
        args.push_back(arg);
    } else if (name == Names::default_arg) {
        SimpleVariant *default_value = it.peek(3);
        SemTypes::FunctionArgument &arg = SemTypes::FunctionArgument(type, id, false).set_default_value(default_value);
        args.push_back(arg);
    } else {
        except("Da fuck");
    }
    last = 0;
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
    if (this->custom_match != nullptr) {
        return this->custom_match(it);
    }
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
                return pos+1;
            pos++;
        }
    }
    if (pos < this->pattern.size())
        return 0;
    return pos;
}

bool Pattern::check_pattern(std::string str, SimpleVariant *variant) {
    if (str == "ANY")
        return true;
    bool by_text = str.at(0) == '!';
    bool by_block = str.at(0) == '$';
    bool not_ = str.at(0) == '_';

    if (by_text || by_block || not_) {
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
    } else if (variant->type == SIMPLE_OBJECT && !by_text && !by_block) {
        auto pToken = reinterpret_cast<Token*>(variant->memptr);
        bool ret = pToken->type == std::stoi(str);
        return not_ == !ret;
    }
    return false;
}

void Codegen::submit_function_create(SemTypes::FunctionDefine &function) {

}

void Codegen::submit_variable_define(SemTypes::VariableDefine &define) {

}

void Codegen::submit_variable_assign(SemTypes::VariableAssign &assign) {

}

std::string Codegen::build() {
    return "";
}