//
// Created by kvxmmu on 08.04.2020.
//

#include <lang/parser/parser.hpp>


namespace {
    const std::string skip_characters = " \t\b\n";
    const std::string id_characters = "_0123456789qwertyuiopasdfghjklzxcvbnm";

    typedef struct {
        std::string value;
        token_type_t meaning;
    } Operator;

    typedef struct {
        std::string value;
        token_type_t meaning;
    } Construction;

    const std::vector<Operator> operators = {
            Operator{"+", PLUS},
            Operator{"-", MINUS},
            Operator{"==", EQUALS},
            Operator{"!=", NOT_EQUALS},
            Operator{"<=", LESS_OR_EQUAL},
            Operator{">=", GREATER_OR_EQUAL},
            Operator{"<", LESS},
            Operator{">", GREATER},
            Operator{"**", POW},
            Operator{"*", MUL},
            Operator{"/", DIV},
            Operator{"%", MOD}
    };

    const std::vector<Construction> syntax_constructions = {
            Construction{"->", RIGHT_ARROW},
            Construction{"<-", LEFT_ARROW},
            Construction{"func", FUNC},
            Construction{".", POINT},
            Construction{"import", IMPORT}, // keyword
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

std::string repeat(const std::string &src, size_t count) {
    if (count == 0)
        return "";
    else if (count == 1)
        return src;
    std::string out;

    for (size_t _ = 0; _ < count; _++) {
        out += src;
    }

    return out;
}

void debug_print(AstTree &tree, size_t indent) {
    std::string indentation = repeat("    ", indent);
    std::string chars;
    for (SimpleVariant *variant : tree.tree) {
        if (variant->type == TREE) {
            auto ptr = reinterpret_cast<AstTree*>(variant->memptr);
            chars = get_block_type(ptr->block_type);
            std::cout << indentation << chars[0] << std::endl;
            debug_print(*ptr, indent+1);
            std::cout << indentation << chars[1] << std::endl;
        } else if (variant->type == SIMPLE_OBJECT) {
            auto tk = reinterpret_cast<Token*>(variant->memptr);
            std::cout << indentation << tk->token << std::endl;
        }
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

bool is_float(tokenizer_args) {
    bool floating_point_parsing = false;

    std::string num;

    char character;
    size_t pos = 0;

    while (!it.is_done()) {
        character = it.peek(pos);
        if (character == '.') {
            if (floating_point_parsing)
                return false;
            floating_point_parsing = true;
            num += '.';
        } else if (isdigit(character)) {
            num += character;
        } else
            break;
        pos++;
    }

    if (!floating_point_parsing || num == "." || num.empty())
        return false;
    else if (num.at(num.size() - 1) == '.') {
        num += '0';
    } else if (num.at(0) == '.')
        num = "0" + num;

    tokens.push_back(Token{num, FLOAT});
    it.next(pos - 1);
    return true;
}

bool is_operator(tokenizer_args) {
    std::string buff;
    for (const Operator &eop : operators) {
        std::string op = eop.value;
        buff = peek_count(it, op.size());
        if (buff == op) {
            it.next(op.size() - 1);
            tokens.push_back(Token{op,
                                   eop.meaning});
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
    for (const Construction &constr : syntax_constructions) {
        std::string construction = constr.value;
        std::string tmp = peek_count(it, construction.size());
        if (tmp == construction) {
            it.next(construction.size() - 1);
            tokens.push_back(Token{construction, constr.meaning});
            return true;
        }
    }
    return false;
}


bool is_string(tokenizer_args) {
    char character = it.peek(0);
    char next_character = it.peek(1);
    bool interpolation = false;
    if (character == 'i' && next_character == '"') {
        interpolation = true;
        it.next();
    } else if (character != '"')
        return false;
    it.next();

    std::string string;

    while (!it.is_done()) {
        character = it.peek(0);
        if (character == '\\') {
            next_character = it.peek(1);
            if (next_character == 'n') {
                string += '\n';
            } else if (next_character == 't') {
                string += '\t';
            } else if (next_character == 'b') {
                string += '\b';
            } else if (next_character == '\\' || next_character == '"') {
                string += next_character;
            }
            it.next();
        } else if (character == '"') {
            break;
        } else {
            string += character;
        }

        it.next();
    }
    if (interpolation)
        tokens.push_back(Token{string, INTERPOLATED_STRING});
    else
        tokens.push_back(Token{string, STRING});
    return true;
}


bool is_character(tokenizer_args) {
    char current = it.peek(0);
    char character = it.peek(1);
    char close_expr = it.peek(2);
    if (current != '\'' || close_expr != '\'' || character == '\'')
        return false;
    tokens.push_back(Token{std::string(&character, 1), CHAR});
    it.next(2);
    return true;
}

bool is_oneline_comment(tokenizer_args) {
    char character = it.peek(0);
    char next_char = it.peek(1);

    std::string comment;

    if (character != '/' || next_char != '/')
        return false;

    it.next(2);
    while (!it.is_done()) {
        character = it.peek();
        if (character == '\n')
            break;
        comment += character;
        it.next();
    }

    tokens.push_back(Token{comment, ONELINE_COMMENT});

    return true;
}

bool is_comment(tokenizer_args) {
    char character = it.peek(0);
    char next_char = it.peek(1);
    if (character != '/' || next_char != '*')
        return false;

    it.next(2);

    std::string comment;

    while (!it.is_done()) {
        character = it.peek(0);
        next_char = it.peek(1);
        if (character == '*' && next_char == '/') {
            it.next();
            break;
        }
        comment += character;
        it.next();
    }
    tokens.push_back(Token{comment, MULTILINE_COMMENT});
    return true;
}


void error_callback(size_t pos, const std::string &data) {
    std::cout << "RedLang SyntaxError: Unexpected character '" << data[pos] << "'" << std::endl;
    exit(0);
}

bool is_assignment_or_semicolon(tokenizer_args) {
    char chr = it.peek(0);
    bool check = chr == ';' || chr == '=' || chr == ',';
    if (!check)
        return check;
    if (chr == ';') {
        tokens.push_back(Token{";", SEMICOLON});
    } else if (chr == '=') {
        tokens.push_back(Token{"=", ASSIGNMENT});
    } else if (chr == ',') {
        tokens.push_back(Token{",", COMMA});
    } else if (chr == ':') {
        tokens.push_back(Token{":", COLON});
    }
    return check;
}


std::vector<Token> lex(const std::string &data) {
    std::vector<Token> tokens;
    RedIterator<std::string, char> it(data, 0);
    while (!it.is_done()) {
        if (is_skip_character(auto_place)) {}
        else if (is_string(auto_place)) {}
        else if (is_character(auto_place)) {}
        else if (is_syntax_construction(auto_place)) {}
        else if (is_id(auto_place)) {}
        else if (is_float(auto_place)) {}
        else if (is_digit(auto_place)) {}
        else if (is_oneline_comment(auto_place)) {}
        else if (is_comment(auto_place)) {}
        else if (is_assignment_or_semicolon(auto_place)) {}
        else if (is_operator(auto_place)) {}
        else if (is_expr(auto_place)) {}
        else if (is_block(auto_place)) {}
        else {
            error_callback(it.pos, data);
        }
        it.next();
    }
    return tokens;
}

void parse_brace(parser_args) {
    bool closed = false;

    while (!it.is_done()) {
        Token tkn = it.peek(0);
        if (tkn.type == OPEN_BRACE) {
            it.next();
            parse_brace(pauto(BRACE));
        } else if (tkn.type == SBRACKET_OPEN) {
            it.next();
            parse_sbrackets(pauto(SBRACKET));
        } else if (tkn.type == OPEN_BRACKET) {
            it.next();
            parse_brackets(pauto(BRACKET));
        } else if (tkn.type == CLOSE_BRACE) {
            closed = true;
            break;
        } else {
            tree.new_token(tkn);
        }
        it.next();
    }
    if (!closed)
        expecting_error("}");
}

void parse_brackets(parser_args) {
    bool closed = false;

    while (!it.is_done()) {
        Token tkn = it.peek(0);
        if (tkn.type == OPEN_BRACE) {
            it.next();
            parse_brace(pauto(BRACE));
        } else if (tkn.type == SBRACKET_OPEN) {
            it.next();
            parse_sbrackets(pauto(SBRACKET));
        } else if (tkn.type == OPEN_BRACKET) {
            it.next();
            parse_brackets(pauto(BRACKET));
        } else if (tkn.type == CLOSE_BRACKET) {
            closed = true;
            break;
        } else {
            tree.new_token(tkn);
        }
        it.next();
    }
    if (!closed)
        expecting_error(")");
}

void parse_sbrackets(parser_args) {
    bool closed = false;

    while (!it.is_done()) {
        Token tkn = it.peek(0);
        if (tkn.type == OPEN_BRACE) {
            it.next();
            parse_brace(pauto(BRACE));
        } else if (tkn.type == SBRACKET_OPEN) {
            it.next();
            parse_sbrackets(pauto(SBRACKET));
        } else if (tkn.type == OPEN_BRACKET) {
            it.next();
            parse_brackets(pauto(BRACKET));
        } else if (tkn.type == SBRACKET_CLOSE) {
            closed = true;
            break;
        } else {
            tree.new_token(tkn);
        }
        it.next();
    }
    if (!closed)
        expecting_error("]");
}

AstTree parse(const std::vector<Token> &tokens) {
    RedIterator<std::vector<Token>, Token> it(tokens, Token{});
    AstTree tree(10, nullptr);
    while (!it.is_done()) {
        Token tkn = it.peek(0);
        if (tkn.type == OPEN_BRACE) {
            it.next();
            parse_brace(pauto(BRACE));
        } else if (tkn.type == SBRACKET_OPEN) {
            it.next();
            parse_sbrackets(pauto(SBRACKET));
        } else if (tkn.type == OPEN_BRACKET) {
            it.next();
            parse_brackets(pauto(BRACKET));
        } else {
            tree.new_token(tkn);
        }
        it.next();
    }
    return tree;
}
