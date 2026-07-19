/* ==============================================
 * This file is under MIT License.
 * For details, see LICENSE.md
 *
 * Copyright (c) 2026 Arseniy Finevich
 * ==============================================
 */

module;

export module tree;

namespace containers {

template <class _Pointer>
class tree_end_node;

template <class _VoidPtr>
class tree_node_base;

template <class _Tp, class _VoidPtr>
class tree_node;

template <class _Key, class _Value>
class value_type;

template <class _NodePtr>
constexpr bool tree_is_left_child(_NodePtr x) noexcept {
    return x == x->parent_->left_;
}

} // namespace containers