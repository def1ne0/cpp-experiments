/* ==============================================
 * This file is under MIT License.
 * For details, see LICENSE.md
 *
 * Copyright (c) 2026 Arseniy Finevich
 * ==============================================
 */

#include <print>

import examples.rb_tree_ex;

template <typename T>
void print_tree(const typename examples::RB_tree<T>::Node* root) {
    if (root == nullptr) return;

    std::print("{}({}) ", root->key_, root->color_ == examples::Color::RED ? "R" : "B");
    print_tree<T>(root->left_);
    print_tree<T>(root->right_);
}

int main() {
    examples::RB_tree<int> tree{1};
    tree.insert(2);
    tree.insert(3);
    tree.insert(4);
    print_tree<int>(tree.get_root());

    return 0;
}