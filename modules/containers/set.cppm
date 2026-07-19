/* ==============================================
 * This file is under MIT License.
 * For details, see LICENSE.md
 *
 * Copyright (c) 2026 Arseniy Finevich
 * ==============================================
 */

module;

#include <functional> // for std::less

export module set;

namespace containers {

export
template <
    class Key,
    class Compare = std::less<Key>,
    class Allocator
> class set final {

};

} // namespace containers