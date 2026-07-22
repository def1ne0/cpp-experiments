/* ==============================================
 * This file is under MIT License.
 * For details, see LICENSE.md
 *
 * Copyright (c) 2026 Arseniy Finevich
 * ==============================================
 */

module;

#include <utility>

export module examples.rb_tree_ex;

namespace examples {

export
enum class Color {
    RED,
    BLACK,
};

export
template <typename T>
class RB_tree final {
public:
    struct Node {
        T key_;
        Color color_;
        Node* left_, *right_, *parent_;
    };

private:
    Node* NIL{nullptr};
    Node* root_{nullptr};

    void clear_tree(Node* node) noexcept;

    // Правый поворот относительно вершины g
    void right_rotate(Node* g);

    // Левый поворот относительно вершины g
    void left_rotate(Node* g);

    // Чинит красно-черное дерево после вставки
    void fix_insertion(Node* v);

    // Заменяет поддерево u на поддерево v
    void transplant(Node* u, Node* v);

    // Находит вершину с минимальным ключов с корнем node
    const Node* minimum(Node* node);

public:
    template <class U>
        requires (!std::same_as<std::decay_t<U>, RB_tree>)
    explicit RB_tree(U&& key)
        : NIL(new Node{T{}, Color::BLACK})
        , root_(new Node{std::forward<U>(key), Color::BLACK, NIL, NIL, NIL}) {
        NIL->parent_ = NIL;
    }

    ~RB_tree() {
        clear_tree(root_);
        delete NIL;
    }

    template <class U>
    void insert(U&& key);

    // TODO
    bool erase(const T& key);

    inline const Node* get_root() const {
        return root_;
    }
};

template <class T>
void RB_tree<T>::clear_tree(Node* node) noexcept {
    if (node == NIL) {
        return;
    }

    clear_tree(node->left_);
    clear_tree(node->right_);

    delete node;
}

template <class T>
void RB_tree<T>::right_rotate(Node* g) {
    //              g                         p
    //            /   \                     /   \
    //           p     u       ----->      1      g
    //         /  \   /  \                      /   \
    //        1   2   3   4                    2     u
    //                                             /   \
    //                                            3     4
    //

    auto* p = g->left_; // Его мы подымем на верх.

    //         Исходное дерево:         Храним:
    //             ...
    //              g                      p
    //            /   \                   /
    //           2     u                 1
    //                /  \
    //               3    4
    //

    g->left_ = p->right_; // Переносим 2 на место p
    if (p->right_ != NIL) {
        p->right_->parent_ = g; // нужно обновить родителя у 2, если это не фиктивный узел
    }

    //          Исходное дерево:        Храним:
    //             ...
    //              p                      g
    //            /                      /   \
    //           1                      2     u
    //                                       /  \
    //                                      3    4

    p->parent_ = g->parent_; // Поднимаем p на место g
    if (g->parent_ == NIL) {
        // Значит g - корень, теперь корень p.
        root_ = p;
    }
    else if (g == g->parent_->left_) {
        // Значит g находится слева от своего отца
        g->parent_->left_ = p;
    }
    else {
        // Значит g находится справа от своего отца
        g->parent_->right_ = p;
    }

    // Опускаем g и делаем его ребенком p
    //             ...
    //              p
    //            /   \
    //           1     g
    //               /  \
    //              2    u
    //                 /   \
    //                3     4

    p->right_ = g;
    g->parent_ = p;
}

template <class T>
void RB_tree<T>::left_rotate(Node* g) {
    //              g                         u
    //            /   \                     /   \
    //           p     u       ----->      g     4
    //         /  \   /  \               /   \
    //        1   2   3   4             p     3
    //                                /  \
    //                               1    2
    //
    auto* u = g->right_; // Тот, кто поднимается

    //         Исходное дерево:         Храним:
    //             ...
    //              g                      u
    //            /   \                      \
    //           p     3                      4
    //         /  \
    //        1    2
    //

    g->right_ = u->left_; // Ставим 3 на место u
    if (u->left_ != NIL) {
        // Нужно обновить родителя у 3, если это не фиктивный узел
        u->left_->parent_ = g;
    }

    //         Исходное дерево:         Храним:
    //             ...
    //              u                      g
    //                \                  /   \
    //                 4                p     3
    //                                /  \
    //                               1    2
    //

    u->parent_ = g->parent_;
    if (g->parent_ == NIL) {
        // Значит g - корень и u станет корнем
        root_ = u;
    }
    else if (g == g->parent_->left_) {
        // Значит g находится слева от своего родителя
        g->parent_->left_ = u;
    }
    else {
        // Значит g находится справа от своего родителя
        g->parent_->right_ = u;
    }

    // Опускаем g и делаем его ребенком u
    //              ...
    //               u
    //             /   \
    //            g     4
    //          /   \
    //         p     3
    //       /  \
    //      1    2
    //

    u->left_ = g;
    g->parent_ = u;
}

template <class T>
void RB_tree<T>::fix_insertion(Node* v) {
    // Нарушается свойство 3: у каждого красного сын черный.
    // Поэтому p красный в нашем случае,
    // а g должен быть черным
    while (v != root_ && v->parent_->color_ == Color::RED) {
        auto* p = v->parent_; // отец v - красный(R)
        auto* g = p->parent_; // дедушка v - черный(B)

        if (p == g->left_) {
            // Если такая картинка, дядя справа
            //              g(B)                      g(B)
            //             /    \                    /    \
            //           p(R)   u(?)     ЛИБО      p(R)   u(?)
            //          /                             \
            //        v(R)                            v(R)

            auto* u = g->right_; // дядя

            // Смотрим на цвет u
            if (u->color_ == Color::RED) {
                // Случай 1.
                // Если дядя красный - u(R), то надо всех троих - p, g, u - перекрасить.
                // Здесь неважно, находятся ли v и p по одну сторону от своих родителей, или нет.
                // Для определенности, пусть находятся.
                // Значит, p(B), g(R), u(B). Передаем проблему рекурсивно
                //              g(B)                      g(R)
                //             /    \                    /    \
                //           p(R)   u(R)  ------>      p(B)   u(B)
                //          /                         /
                //        v(R)                      v(R)

                // Свапаем цвета непосредственно
                p->color_ = Color::BLACK;
                g->color_ = Color::RED;
                u->color_ = Color::BLACK;

                // Двигаемся вверх
                v = g;
            }
            else {
                // Случай 2.
                // Значит дядя черный - u(B).
                //              g(B)                      g(B)
                //             /    \                    /    \
                //           p(R)   u(B)     ЛИБО      p(R)   u(B)
                //          /                             \
                //        v(R)                            v(R)
                if (v == p->right_) {
                    // Случай 2.2.
                    // v и p по разные стороны от своих родителей.
                    // Сделаем лефт ротейт.
                    //              g(B)                       g(B)
                    //             /    \                     /    \
                    //           p(R)   u(B)   ----->       v(R)   u(B)
                    //              \                       /
                    //              v(R)                  p(R)
                    // Пришли к случаю 2.1 и фиксим его
                    v = p; // Обновляем v, т.к. после поворота старый указатель p станет p
                    left_rotate(v);
                    // После поворота p и v поменялись местами в дереве,
                    // но нужно обновить p и g для случая 2.1
                    p = v->parent_;
                    g = p->parent_;
                }

                // Случай 2.1.
                // v и p по одну сторону от своих родителей.
                // Нужен райт ротейт + свап цветов у g, p.
                // Причем все починится и можно возвращаться.
                //              g(B)                       p(B)
                //             /    \                     /    \
                //           p(R)   u(B)   ----->       v(R)   g(R)
                //          /                                     \
                //        v(R)                                    u(B)
                g->color_ = Color::RED;
                p->color_ = Color::BLACK;
                right_rotate(g);
                return;
            }

        }
        else {
            // Дядя слева, все одно и то же, лишь зеркально.
            // Если такая картинка, дядя слева
            //              g(B)                      g(B)
            //             /    \                    /    \
            //           u(?)   p(R)     ЛИБО      u(?)   p(R)
            //                     \                      /
            //                     v(R)                 v(R)
            auto* u = g->left_;

            if (u->color_ == Color::RED) {
                p->color_ = Color::BLACK;
                g->color_ = Color::RED;
                u->color_ = Color::BLACK;
                v = g;
            }
            else {
                if (v == p->left_) {
                    v = p;
                    right_rotate(v);
                    p = v->parent_;
                    g = p->parent_;
                }

                p->color_ = Color::BLACK;
                g->color_ = Color::RED;
                left_rotate(g);
                return;
            }
        }
    }

    // Корень всегда черный, из-за случая 1 и рекурсии он мог стать красным.
    root_->color_ = Color::BLACK;
}

template <typename T>
void RB_tree<T>::transplant(Node* u, Node* v) {
    // Меняет поддерево u на поддерево v
    //        g                  g
    //      /   \      ->      /   \
    //     u     ?            v      ?
    //
    //              ЛИБО
    //        g                  g
    //      /   \      ->      /   \
    //     ?     u            ?     v
    //

    auto* g = u->parent_;

    if (g == NIL) {
        // Значит, g - корень
        root_ = v;
    }
    else if (g->left_ == u) {
        // Значит u - левый ребенок g
        g->left_ = v;
    }
    else {
        // Значит u - правый ребенок g
        g->right_ = v;
    }

    v->parent_ = g;
}

template <typename T>
const typename RB_tree<T>::Node* RB_tree<T>::minimum(Node* node) {
    while (node->left_ != NIL) {
        node = node->left_;
    }

    return node;
}

template <class T>
template <class U>
void RB_tree<T>::insert(U&& key) {
    Node* temp = new Node{std::forward<U>(key), Color::RED, NIL, NIL, NIL};

    auto* p = root_;
    auto* q = NIL;

    // Спускаемся по дереву, пока не найдем нужный лист
    while (p != NIL) {
        q = p;

        if (temp->key_ > p->key_)
            p = p->right_;
        else
            p = p->left_;
    }

    temp->parent_ = q;

    if (temp->key_ > q->key_)
        q->right_ = temp;
    else
        q->left_ = temp;

    fix_insertion(temp);
}

// TODO
/*
template <class T>
bool RB_tree<T>::erase(const T& key) {
    auto* z = root_;

    if (key < z->key_) {
        z = z->left_;
    }
    else {
        z = z->right_;
    }

    if (z == NIL) {
        return false;
    }

    auto* y = z;
    Node* x;
    auto orig_color_y = y->color_;
}
*/

} // namespace examples