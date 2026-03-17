#ifndef __MORLOC__SET_HPP__
#define __MORLOC__SET_HPP__

#include <set>
#include <vector>
#include <functional>
#include <type_traits>
#include <optional>
#include <stdexcept>

// --- Pack/Unpack ---

template <class A>
std::set<A> morloc_packSet(const std::vector<A>& items) {
    return std::set<A>(items.begin(), items.end());
}

template <class A>
std::vector<A> morloc_unpackSet(const std::set<A>& s) {
    return std::vector<A>(s.begin(), s.end());
}

// --- Construction ---

// singleton :: a -> Set a
template <class A>
std::set<A> morloc_singleton(const A& x) {
    return std::set<A>{x};
}

// fromList :: [a] -> Set a
template <class A>
std::set<A> morloc_fromList(const std::vector<A>& xs) {
    return std::set<A>(xs.begin(), xs.end());
}

// toList :: Set a -> [a]
template <class A>
std::vector<A> morloc_toList(const std::set<A>& s) {
    return std::vector<A>(s.begin(), s.end());
}

// --- Query ---

// member :: a -> Set a -> Bool
template <class A>
bool morloc_member(const A& x, const std::set<A>& s) {
    return s.count(x) > 0;
}

// size :: Set a -> Int
template <class A>
int morloc_size(const std::set<A>& s) {
    return static_cast<int>(s.size());
}

// --- Modify ---

// insertSet :: a -> Set a -> Set a
template <class A>
std::set<A> morloc_insertSet(const A& x, const std::set<A>& s) {
    std::set<A> result(s);
    result.insert(x);
    return result;
}

// deleteSet :: a -> Set a -> Set a
template <class A>
std::set<A> morloc_deleteSet(const A& x, const std::set<A>& s) {
    std::set<A> result(s);
    result.erase(x);
    return result;
}

// --- Set Operations ---

// union :: Set a -> Set a -> Set a
template <class A>
std::set<A> morloc_union(const std::set<A>& a, const std::set<A>& b) {
    std::set<A> result(a);
    result.insert(b.begin(), b.end());
    return result;
}

// intersection :: Set a -> Set a -> Set a
template <class A>
std::set<A> morloc_intersection(const std::set<A>& a, const std::set<A>& b) {
    std::set<A> result;
    for (const auto& elem : a) {
        if (b.count(elem) > 0) {
            result.insert(elem);
        }
    }
    return result;
}

// difference :: Set a -> Set a -> Set a
template <class A>
std::set<A> morloc_difference(const std::set<A>& a, const std::set<A>& b) {
    std::set<A> result;
    for (const auto& elem : a) {
        if (b.count(elem) == 0) {
            result.insert(elem);
        }
    }
    return result;
}

// symmetricDifference :: Set a -> Set a -> Set a
template <class A>
std::set<A> morloc_symmetricDifference(const std::set<A>& a, const std::set<A>& b) {
    std::set<A> result;
    for (const auto& elem : a) {
        if (b.count(elem) == 0) {
            result.insert(elem);
        }
    }
    for (const auto& elem : b) {
        if (a.count(elem) == 0) {
            result.insert(elem);
        }
    }
    return result;
}

// --- Predicates ---

// isSubset :: Set a -> Set a -> Bool
template <class A>
bool morloc_isSubset(const std::set<A>& a, const std::set<A>& b) {
    for (const auto& elem : a) {
        if (b.count(elem) == 0) {
            return false;
        }
    }
    return true;
}

// isDisjoint :: Set a -> Set a -> Bool
template <class A>
bool morloc_isDisjoint(const std::set<A>& a, const std::set<A>& b) {
    for (const auto& elem : a) {
        if (b.count(elem) > 0) {
            return false;
        }
    }
    return true;
}

// --- Transform ---

// filterSet :: (a -> Bool) -> Set a -> Set a
template <class A, class F>
std::set<A> morloc_filterSet(F f, const std::set<A>& s) {
    std::set<A> result;
    for (const auto& elem : s) {
        if (f(elem)) {
            result.insert(elem);
        }
    }
    return result;
}

// mapSet :: (a -> b) -> Set a -> Set b
template <class A, class F>
auto morloc_mapSet(F f, const std::set<A>& s)
    -> std::set<std::invoke_result_t<F, const A&>>
{
    using B = std::invoke_result_t<F, const A&>;
    std::set<B> result;
    for (const auto& elem : s) {
        result.insert(f(elem));
    }
    return result;
}

// --- Foldable ---

// fold :: (b -> a -> b) -> b -> Set a -> b
template <class F, class A, class B>
B morloc_fold_set(F&& f, B init, const std::set<A>& s) {
    B acc = init;
    for (const auto& elem : s) {
        acc = f(acc, elem);
    }
    return acc;
}

// fold1 :: (a -> a -> a) -> Set a -> a
template <class F, class A>
A morloc_fold1_set(F&& f, const std::set<A>& s) {
    if (s.empty()) {
        throw std::runtime_error("fold1 on empty set");
    }
    auto it = s.begin();
    A acc = *it;
    ++it;
    for (; it != s.end(); ++it) {
        acc = f(acc, *it);
    }
    return acc;
}

// safeFold1 :: (a -> a -> a) -> Set a -> ?a
template <class F, class A>
std::optional<A> morloc_safeFold1_set(F&& f, const std::set<A>& s) {
    if (s.empty()) {
        return std::nullopt;
    }
    auto it = s.begin();
    A acc = *it;
    ++it;
    for (; it != s.end(); ++it) {
        acc = f(acc, *it);
    }
    return acc;
}

// --- Monoid ---

// emptySet :: Set a
// Uses a factory with implicit conversion to work around C++ template
// deduction for nullary functions.
struct MorlocEmptySetFactory {
    template <class A>
    operator std::set<A>() const { return std::set<A>(); }
};

inline MorlocEmptySetFactory morloc_emptySet() {
    return MorlocEmptySetFactory{};
}

#endif
