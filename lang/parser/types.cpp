//
// Created by kvxmmu on 08.04.2020.
//

#include <lang/parser/types.hpp>


void move_tree(const AstTree *ast, AstTree *dst) {
    AstTree *nmem;
    Token *tmem;
    for (SimpleVariant *node : ast->tree) {
        if (node->type == TREE) {
            nmem = dst->new_tree();
            move_tree(reinterpret_cast<AstTree*>(node->memptr), nmem);
        } else {
            tmem = reinterpret_cast<Token*>(node->memptr);
            dst->new_token(Token{
                tmem->token,
                tmem->type
            });
        }
    }
}

SimpleVariant *create_variant(const AstTree* ast) {
    auto variant = new SimpleVariant;
    variant->memptr = new AstTree();
    variant->type = TREE;
    move_tree(ast, reinterpret_cast<AstTree*>(variant->memptr));
    return variant;
}

SimpleVariant *create_variant(const Token &tkn) {
    auto *variant = new SimpleVariant;
    variant->memptr = new Token{
        tkn.token,
        tkn.type
    };
    variant->type = SIMPLE_OBJECT;
    return variant;
}


AstTree::AstTree(size_t alloc_size, SimpleVariant *__variant) : variant(__variant) {
    this->tree.reserve(alloc_size);
}

AstTree *AstTree::new_tree(token_type_t token_type) {
    auto *ntree = new AstTree();
    ntree->block_type = token_type;
    ntree->need_free = true;
    ntree->variant = nullptr;

    auto pVariant = new SimpleVariant;
    pVariant->type = TREE;
    pVariant->memptr = ntree;

    this->tree.push_back(pVariant);
    return ntree;
}


Token *AstTree::new_token(const Token &tkn_ref) {
    auto pVariant = create_variant(tkn_ref);
    this->tree.push_back(pVariant);
    return reinterpret_cast<Token*>(pVariant->memptr);
}

void AstTree::clear_tree() {

    AstTree *pAst;
    Token *pToken;

    for (SimpleVariant *pVariant : this->tree) {
        if (pVariant->type == TREE) {
            pAst = reinterpret_cast<AstTree*>(pVariant->memptr);
            pAst->dealloc();
            delete pAst;
        } else {
            pToken = reinterpret_cast<Token*>(pVariant->memptr);
            delete pToken;
        }
        delete pVariant;
    }

}
