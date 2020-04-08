#include <iostream>
#include <lang/parser/types.hpp>

int main() {
    AstTree tree(10, nullptr);
    tree.new_token(Token{
        "test tree",
        1
    });
    tree.new_tree();
    tree.clear_tree();
    /*
     * Alloc & dealloc test
     */
    return 0;
}
