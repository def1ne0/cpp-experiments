/* ==============================================
 * This file is under MIT License.
 * For details, see LICENSE.md
 *
 * Copyright (c) 2026 Arseniy Finevich
 * ==============================================
 */

module;

export module containers.tree;

namespace containers {

template <class Pointer>
class tree_end_node;

template <class VoidPtr>
class tree_node_base;

template <class Tp, class VoidPtr>
class tree_node;

template <class Key, class Value>
class value_type;

template <class NodePtr>
constexpr bool tree_is_left_child(NodePtr x) noexcept {
    return x == x->parent_->left_;
}

template <class NodePtr>
constexpr unsigned tree_sub_invariant(NodePtr x) {
    if (x == nullptr)
        return 1;

    if (x->left_ != nullptr && x->left_->parent_ != x)
        return 0;

    if (x->right_ != nullptr && x->right_->parent_ != x)
        return 0;

    if (x->left_ == x->right_ && x->left_ != nullptr)
        return 0;

    if (!x->is_black_) {
        if (x->left_ && !x->left_->is_black_)
            return 0;
        if (x->right_ && !x->right_->is_black_)
            return 0;
    }

    auto h = tree_sub_invariant(x->left_);

    if (h == 0)
        return 0;
    if (h != tree_sub_invariant(x->right_))
        return 0;

    return h + x->is_black_;
}

} // namespace containers