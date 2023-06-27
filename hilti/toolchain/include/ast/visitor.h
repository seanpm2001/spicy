// Copyright (c) 2020-2021 by the Zeek Project. See LICENSE for details.

#pragma once

#include <utility>
#include <vector>

#include <hilti/ast/node.h>
#include <hilti/ast/visitor-dispatcher.h>

namespace hilti {

namespace detail::visitor {

enum class Order { Pre, Post };

template<Order order>
class Iterator {
public:
    using value_type = NodePtr;

    Iterator() = default;
    Iterator(const NodePtr& root, bool include_empty = false) : _include_empty(include_empty) {
        if ( root )
            _path.emplace_back(root, -1);
    }

    Iterator(const Iterator& other) = default;
    Iterator(Iterator&& other) noexcept = default;

    ~Iterator() = default;

    auto depth() const { return _path.size(); }

    Iterator& operator++() {
        next();
        return *this;
    }

    NodePtr operator*() const { return current(); }

    Iterator& operator=(const Iterator& other) = default;
    Iterator& operator=(Iterator&& other) noexcept = default;
    bool operator==(const Iterator& other) const { return _path == other._path; }
    bool operator!=(const Iterator& other) const { return ! (*this == other); }

private:
    struct Location {
        NodePtr node;
        int child = -2;

        Location(NodePtr n, int c) : node(std::move(n)), child(c) {}
        auto operator==(const Location& other) const {
            return std::pair(node, child) == std::pair(other.node, other.child);
        }
    };

    NodePtr current() const {
        if ( _path.empty() )
            throw std::runtime_error("invalid reference of visitor's iterator");

        auto& p = _path.back();

        if ( ! p.node )
            return nullptr;

        if ( p.child < 0 ) {
            assert(order == Order::Pre);
            return p.node;
        }

        if ( p.child == static_cast<int>(p.node->children().size()) ) {
            assert(order == Order::Post);
            return p.node;
        }

        assert(p.child < static_cast<int>(p.node->children().size()));
        return p.node->children()[p.child];
    }

    void next() {
        if ( _path.empty() )
            return;

        auto& p = _path.back();
        p.child += 1;

        if ( p.child == -1 ) {
            if ( order == Order::Pre || (p.node && p.node->pruneWalk()) )
                return;

            next();
            return;
        }

        if ( ! p.node ) {
            _path.pop_back();
            next();
            return;
        }

        if ( p.node->pruneWalk() ) {
            _path.pop_back();
            next();
            return;
        }

        assert(p.child >= 0);

        if ( p.child < static_cast<int>(p.node->children().size()) ) {
            if ( auto child = p.node->children()[p.child]; child || _include_empty ) // don't visit null children
                _path.emplace_back(child, -2);

            next();
            return;
        }

        if ( p.child == static_cast<int>(p.node->children().size()) ) {
            if constexpr ( order == Order::Post )
                return;

            p.child += 1;
        }

        if ( p.child > static_cast<int>(p.node->children().size()) ) {
            _path.pop_back();
            next();
            return;
        }
    }

    std::vector<Location> _path;
    bool _include_empty = false;
};

template<Order order>
class Range {
public:
    using iterator_t = Iterator<order>;
    using value_type = typename Iterator<order>::value_type;
    Range(NodePtr root) : _root(std::move(root)) {}

    auto begin(bool include_empty = false) {
        if constexpr ( order == Order::Pre )
            return iterator_t(_root, include_empty);

        return ++iterator_t(_root, include_empty);
    }

    auto end() { return iterator_t(); }

private:
    NodePtr _root;
};

/**
 * AST visitor.
 *
 * @tparam order order of iteration
 */
template<Order order>
class Visitor : public ::hilti::visitor::Dispatcher {
public:
    using base_t = Visitor<order>;
    using iterator_t = Iterator<order>;
    static const Order order_ = order;

    Visitor() = default;
    virtual ~Visitor() = default;

    /** Execute matching dispatch methods for a single node.  */
    void dispatch(const NodePtr& n) { n->dispatch(*this); }

    // /** Execute matching dispatch methods for a single node.  */
    // void dispatch(Node& n) { n.dispatch(*this); }

    /**
     * Return a range that iterates over AST, returning each node successively.
     *
     * @note The returned range operates on references to the AST passed
     * in, so make sure that stays around as long as necessary.
     */
    auto walk(const NodePtr& root) { return Range<order>(root); }
};

} // namespace detail::visitor

/**
 * Visitor performing a pre-order iteration over an AST.
 */
namespace visitor {
using PreOrder = detail::visitor::Visitor<detail::visitor::Order::Pre>;

/**
 * Iterator range traversing an AST in pre-order.
 */
using RangePreOrder = detail::visitor::Range<detail::visitor::Order::Pre>;

/**
 * Visitor performing a post-order iteration over an AST.
 */
using PostOrder = detail::visitor::Visitor<detail::visitor::Order::Post>;

/**
 * Iterator range traversing an AST in post-order.
 */
using RangePostOrder = detail::visitor::Range<detail::visitor::Order::Post>;

/** Walk the AST recursively and call dispatch for each node. */
template<typename Visitor, typename NodePtr>
auto visit(Visitor&& visitor, NodePtr& root) {
    for ( auto i : visitor.walk(root) )
        visitor.dispatch(i);
}

/** Walk the AST recursively and call dispatch for each node, then run callback to retrieve a result. */
template<typename Visitor, typename NodePtr, typename ResultFunc>
auto visit(Visitor&& visitor, NodePtr& root, ResultFunc result) {
    for ( auto i : visitor.walk(root) )
        visitor.dispatch(i);

    return result(visitor);
}

template<typename Visitor>
void dispatch(Visitor&& visitor, const NodePtr& node, const NodePtr& n) {
    n->dispatch(visitor);
}

template<typename Visitor, typename ResultFunc>
auto dispatch(Visitor&& visitor, const NodePtr& node, ResultFunc result) {
    node->dispatch(visitor);
    return result(visitor);
}

} // namespace visitor
} // namespace hilti
