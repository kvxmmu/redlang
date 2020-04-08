//
// Created by kvxmmu on 08.04.2020.
//

#include <lang/parser/parser.hpp>


namespace {
    const std::string skip_characters = " \t\b\n";
    const std::string id_characters = "_0123456789qwertyuiopasdfghjklzxcvbnm";

    const std::vector<std::string> operators = {
            "+", "-",
            "==", "!=",
            "<=", ">=",
            "<", ">",
            "**", "*",
            "/", "%"
    };

    const std::vector<std::string> syntax_constructions = {
            "->", "<-",
            "func", ".",
            "import", // keyword
            ";"
    };
}

std::string peek_count(RedIterator<std::string, char> &it, size_t count) {
    std::string out;
    size_t peek_step = 0;
    while (!it.is_done(peek_step) && peek_step < count) {
        char character = it.peek(peek_step);
        out += character;
        peek_step++;
    }
    return out;
}

void debug_print(const std::vector<Token> &tokens) {
    for (const Token &token : tokens) {
        std::cout << token.token << ": " << static_cast<int>(token.type) << std::endl;
    }
}


bool any_match(const std::string &buff, const char &c) {
    for (const char &character : buff) {
        if (character == c)
            return true;
    }
    return false;
}


bool is_skip_character(tokenizer_args) {
    char chr = it.peek(0);
    return any_match(skip_characters, chr);
}

bool is_id(tokenizer_args) {
    char first_character = it.peek(0);
    if (isdigit(first_character) || !any_match(id_characters, first_character)) {
        return false;
    }
    std::string out;

    while (!it.is_done()) {
        char current_chr = it.peek(0);
        if (!any_match(id_characters, current_chr)) {
            break;
        } else {
          out += current_chr;
        }
        it.next();
    }
    tokens.push_back(Token{
        out, ID
    });
    it.next(-1);
    return true;
}


bool is_digit(tokenizer_args) {
    std::string num;
    while (!it.is_done()) {
        char character = it.peek(0);
        if (!isdigit(character))
            break;
        else
            num += character;
        it.next();
    }
    if (num.empty())
        return false;
    tokens.push_back(Token{
        num, INTEGER});
    it.next(-1);
    return true;
}

bool is_operator(tokenizer_args) {
    std::string buff;
    for (const std::string &op : operators) {
        buff = peek_count(it, op.size());
        if (buff == op) {
            it.next(op.size() - 1);
            tokens.push_back(Token{op,
                                   OPERATOR});
            return true;
        }
    }
    return false;
}


bool is_block(tokenizer_args) {
    char character = it.peek(0);
    if (character == '{') {
        tokens.push_back(Token{"{", OPEN_BRACE});
    } else if (character == '}') {
        tokens.push_back(Token{"}", CLOSE_BRACE});
    }
    return character == '{' || character == '}';
}



bool is_expr(tokenizer_args) {
    char character = it.peek(0);
    if (character == '(') {
        tokens.push_back(Token{"(", OPEN_BRACKET});
    } else if (character == ')') {
        tokens.push_back(Token{")", CLOSE_BRACKET});
    } else if (character == '[') {
        tokens.push_back(Token{"[", SBRACKET_OPEN});
    } else if (character == ']') {
        tokens.push_back(Token{"]", SBRACKET_CLOSE});
    } else
        return false;
    return true;
}


bool is_syntax_construction(tokenizer_args) {
    for (const std::string &construction : syntax_constructions) {
        std::string tmp = peek_count(it, construction.size());
        if (tmp == construction) {
            it.next(construction.size() - 1);
            tokens.push_back(Token{construction, SYNTAX_CONSTRUCTION});
            return true;
        }
    }
    return false;
}


void error_callback(size_t pos, const std::string &data) {
    std::cout << "RedLang SyntaxError: Unexpected character '" << data[pos] << "'" << std::endl;
    exit(0);
}


std::vector<Token> lex(const std::string &data) {
    std::vector<Token> tokens;
    RedIterator<std::string, char> it(data, 0);
    while (!it.is_done()) {
        if (is_skip_character(auto_place)) {}
        else if (is_id(auto_place)) {}
        else if (is_digit(auto_place)) {}
        else if (is_operator(auto_place)) {}
        else if (is_expr(auto_place)) {}
        else if (is_block(auto_place)) {}
        else if (is_syntax_construction(auto_place)){}
        else {
            error_callback(it.pos, data);
        }
        it.next();
    }
    return tokens;
}
