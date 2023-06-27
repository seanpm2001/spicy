// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <algorithm>
#include <cinttypes>
#include <map>
#include <memory>
#include <ostream>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <hilti/ast/doc-string.h>
#include <hilti/ast/forward.h>
#include <hilti/ast/id.h>
#include <hilti/ast/meta.h>
#include <hilti/ast/node-range.h>
#include <hilti/ast/scope.h>
#include <hilti/ast/visitor-dispatcher.h>

#define HILTI_NODE_BASE(CLASS) void dispatch(visitor::Dispatcher& v) override;

#define HILTI_NODE(CLASS)                                                                                              \
    NodePtr _clone(ASTContext* ctx) const final { return NodeDerivedPtr<CLASS>(new CLASS(*this)); }                    \
    void dispatch(visitor::Dispatcher& v) final;                                                                       \
    friend class hilti::builder::NodeBuilder;

#define HILTI_NODE_IMPLEMENTATION_0(CLASS)                                                                             \
    void ::hilti::CLASS::dispatch(visitor::Dispatcher& v) { v(this); }

#define HILTI_NODE_IMPLEMENTATION_1(CLASS, BASE)                                                                       \
    void ::hilti::CLASS::dispatch(visitor::Dispatcher& v) {                                                            \
        v(static_cast<BASE*>(this));                                                                                   \
        v(this);                                                                                                       \
    }

namespace hilti {
namespace builder {
class NodeBuilder;
}

class Nodes : public std::vector<NodePtr> {
public:
    using std::vector<NodePtr>::vector;

    template<typename T>
    Nodes(std::vector<std::shared_ptr<T>> m) {
        for ( auto&& x : m )
            emplace_back(std::move(x));
    }
};

namespace node {

// Shallow copy.
NodePtr clone(ASTContext* ctx, const NodePtr& n);

// Deep copy.
NodePtr deepClone(ASTContext* ctx, const NodePtr& n);

/** Value of a node property, stored as part of `Properties`. */
using PropertyValue = std::variant<bool, const char*, double, int, int64_t, unsigned int, uint64_t, std::string, ID,
                                   std::optional<std::string>, std::optional<ID>>;

/** Renders a property value into a string for display. */
inline std::string to_string(const PropertyValue& v) {
    struct Visitor {
        auto operator()(bool s) { return std::string(s ? "true" : "false"); }
        auto operator()(const char* s) { return util::escapeUTF8(s); }
        auto operator()(double d) { return util::fmt("%.6f", d); }
        auto operator()(int i) { return util::fmt("%d", i); }
        auto operator()(int64_t i) { return util::fmt("%" PRId64, i); }
        auto operator()(const std::string& s) { return util::escapeUTF8(s); }
        auto operator()(const ID& id) { return id.str(); }
        auto operator()(const std::optional<ID>& id) { return id ? id->str() : "<not set>"; }
        auto operator()(const std::optional<std::string>& s) { return s ? util::escapeUTF8(*s) : "<not set>"; }
        auto operator()(unsigned int u) { return util::fmt("%u", u); }
        auto operator()(uint64_t u) { return util::fmt("%" PRIu64, u); }
    };

    return std::visit(Visitor(), v);
};

/** Importance of reporting an error, relative to others. */
enum class ErrorPriority {
    High = 3,   /**< high priority error that will always be reported */
    Normal = 2, /**< normal priority error that will be reported if there are no higher priority ones */
    Low = 1,    /**< low priority error that will be reported if there are no higher priority ones */
    NoError = 0 /**< place-holder for comparison if no error was encountered */
};

inline bool operator<(ErrorPriority x, ErrorPriority y) {
    return static_cast<std::underlying_type_t<ErrorPriority>>(x) <
           static_cast<std::underlying_type_t<ErrorPriority>>(y);
}

/** Error information associated with nodes. */
struct Error {
    std::string message;                            /**< main error message to report  */
    Location location;                              /**< location associated with the error */
    std::vector<std::string> context;               /**< additional lines to print along with error as context */
    ErrorPriority priority = ErrorPriority::Normal; /**< priortity of error */

    // Comparison considers message & location, so that we can unique based
    // on those two.
    bool operator<(const Error& other) const {
        return std::tie(message, location) < std::tie(other.message, other.location);
    }
};

/**
 * Properties associated with an AST node. A property is a key/value pair
 * recording node-specific, atomic information that's not represented by
 * further child nodes.
 */
using Properties = std::map<std::string, node::PropertyValue>;

bool areEqual(const Node& x, const Node& y);

} // namespace node

class Node : public std::enable_shared_from_this<Node> {
public:
    virtual ~Node();

    virtual node::Properties properties() const { return {}; }

    const auto& children() const { return _children; }
    Node* parent() const { return _parent; }

    const auto& meta() const { return _meta; }
    void setMeta(Meta m) { _meta = std::move(m); }

    /**
     * Returns the scope associated with the node, if any.
     */
    auto scope() const { return _scope.get(); }

    /**
     * Returns the node's direct scope if already created, or creates one if
     * not yet.
     */
    auto getOrCreateScope() {
        if ( ! _scope )
            _scope = std::make_unique<Scope>();

        return _scope.get();
    }

    /** Clears out the current scope. */
    void clearScope() { _scope.reset(); }

    /**
     * Looks an ID in the node's chain of scope, following HILTI's scoping and visibility rules.
     *
     * @param id id to look up
     * @param what description of what we're looking for, for error reporting
     * @return if found, a pair of the declaration the ID refers to plus the declarations canonical ID; if not found an
     * error message appropriate for reporting to the user
     */
    Result<std::pair<DeclarationPtr, ID>> lookupID(const ID& id, const std::string_view& what) const;

    auto pruneWalk() const { return _prune_walk; }
    void setPruneWalk(bool prune) { _prune_walk = prune; }

    bool inheritScope() const { return _inherit_scope; }
    void setInheritScope(bool inherit) { _inherit_scope = inherit; }

    const auto& location() const { return _meta.location(); }
    std::string typename_() const { return typeid(*this).name(); }
    uintptr_t identity() const { return reinterpret_cast<uintptr_t>(this); }

    /**
     * Returns a unique ID for this node.
     *
     * @note This is primarily for internal usage.
     * @todo Not sure if we want to keep this, for now we just redirect to identity()
     */
    auto rid() const { return identity(); }

    /**
     * Returns a string representation of `rid()`.
     *
     * @note This is primarily for internal usage.
     */
    std::string renderedRid() const { return util::fmt("%%%" PRIu64, rid()); }

    /**
     * Returns a child.
     *
     * @tparam T type that the child nodes are assumed to (and must) have
     * @param i index of the child, in the order they were passed into the constructor and/or added
     * @return child casted to type `T`
     */
    template<typename T>
    std::shared_ptr<T> child(int i) const {
        assert(i < _children.size());
        return _children[i] ? _children[i]->as<T>() : nullptr;
    }

    NodePtr child(int i) const {
        assert(i < _children.size());
        return _children[i];
    }

    /**
     * Returns a subrange of children. The indices correspond to the order
     * children were passed into the constructor and/or added.
     *
     * @tparam T type that the child nodes are assumed to (and must) have
     * @param begin index of first child to include; a negative index counts Python-style from end of list
     * @param end index of one beyond last child to include; a negative index counts Python-style from end of list
     * @return range containing children from `start` to `end`
     */
    template<typename T>
    auto children(int begin, int end) const {
        auto end_ = (end < 0) ? _children.end() : _children.begin() + end;
        return hilti::node::Range<T>(_children.begin() + begin, end_);
    }

    template<typename T>
    auto children(int begin, int end) {
        auto end_ = (end < 0) ? _children.end() : _children.begin() + end;
        return hilti::node::Range<T>(_children.begin() + begin, end_);
    }

    /**
     * Returns a subset of children selected by their type.
     *
     * @tparam T type of children to return
     * @return set of all children that have type `T`
     */
    template<typename T>
    hilti::node::Set<T> childrenOfType() const {
        typename hilti::node::Set<T> n;
        for ( auto c = _children.begin(); c != _children.end(); c = std::next(c) ) {
            if ( ! *c )
                continue;

            if ( auto t = (*c)->tryAs<T>() )
                n.insert(t);
        }

        return n;
    }

    /**
     * Adds a child node. It will be appended to the end of the current list
     * node of children.
     */
    void addChild(NodePtr n) {
        if ( ! n->location() && _meta.location() )
            n->setMeta(_meta);

        _children.emplace_back(std::move(n));
        _children.back()->_parent = this;
    }

    void removeChildren(int begin, int end) {
        auto end_ = (end < 0) ? _children.end() : _children.begin() + end;
        for ( auto i = _children.begin() + begin; i < end_; i++ ) {
            if ( *i )
                (*i)->_parent = nullptr;
        }

        _children.erase(_children.begin() + begin, end_);
    }

    void replaceChildren(Nodes children);

    void setChild(size_t idx, NodePtr n) {
        if ( n && ! n->location() && _meta.location() )
            n->setMeta(_meta);

        if ( _children[idx] )
            _children[idx]->_parent = nullptr;

        _children[idx] = std::move(n);

        if ( _children[idx] )
            _children[idx]->_parent = this;
    }

    void destroyChildren();

    template<typename T>
    bool isA() const {
        return typeid(*this) == typeid(T);
    }

    template<typename T>
    auto as() const {
        if ( const auto p = std::dynamic_pointer_cast<const T>(shared_from_this()) )
            return p;

        std::cerr << hilti::util::fmt("internal error: unexpected type, want %s but have %s",
                                      hilti::util::typename_<T>(), typename_())
                  << std::endl;
        hilti::util::abort_with_backtrace();
    }

    template<typename T>
    auto as() {
        if ( auto p = std::dynamic_pointer_cast<T>(shared_from_this()) )
            return p;

        std::cerr << hilti::util::fmt("internal error: unexpected type, want %s but have %s",
                                      hilti::util::typename_<T>(), typename_())
                  << std::endl;
        hilti::util::abort_with_backtrace();
    }

    template<typename T>
    auto tryAs() const {
        return std::dynamic_pointer_cast<const T>(shared_from_this());
    }

    template<typename T>
    auto tryAs() {
        return std::dynamic_pointer_cast<T>(shared_from_this());
    }

    /**
     * Print out a HILTI source code representation of the node and all its
     * children. If the node is not the root of an AST, it's not guaranteed
     * that the result will form valid HILTI source code (but it can still be
     * used, e.g., in error messages).
     *
     * @param out output stream
     * @param compact create a one-line representation
     *
     */
    void print(std::ostream& out, bool compact = false) const;

    /**
     * Returns a HILTI source code representation of the node and all its
     * children. This can be called from inside a debugger.
     */
    std::string print() const;

    /**
     * Returns an internal string representation of the node and all its
     * children.
     *
     * @param include_location if true, include source code locations into
     * the output
     */
    std::string render(bool include_location = true) const;

    /** Returns true if there are any errors associated with the node. */
    bool hasErrors() const { return _errors.size(); }

    /** Returns any error messages associated with the node. */
    const auto& errors() const { return _errors; }

    /** Clears any error message associated with the node. */
    void clearErrors() { _errors.clear(); }

    /**
     * Associate an error message with the node. The error's location will be
     * that of the current node, and it will have normal priority.
     *
     * @param msg error message to report
     * @param context further lines of context to show along with error
     *
     */
    void addError(std::string msg, std::vector<std::string> context = {}) {
        addError(std::move(msg), location(), std::move(context));
    }

    /**
     * Associate an error message with the node. The error's location will be
     * that of the current node.
     *
     * @param msg error message to report
     * @param priority importance of showing the error
     * @param context further lines of context to show along with error
     *
     */
    void addError(std::string msg, node::ErrorPriority priority, std::vector<std::string> context = {}) {
        addError(std::move(msg), location(), priority, std::move(context));
    }

    /**
     * Associate an error message with the node. The error will have normal
     * priority.
     *
     * @param msg error message to report
     * @param l custom location to associate with the error
     * @param context further lines of context to show along with error
     */
    void addError(std::string msg, const Location& l, std::vector<std::string> context = {}) {
        addError(std::move(msg), location(), node::ErrorPriority::Normal, std::move(context));
    }

    /**
     * Associate an error message with the node.
     *
     * @param msg error message to report
     * @param l custom location to associate with the error
     * @param priority importance of showing the error
     * @param context further lines of context to show along with error
     */
    void addError(std::string msg, Location l, node::ErrorPriority priority, std::vector<std::string> context = {}) {
        node::Error error;
        error.message = std::move(msg);
        error.location = std::move(l);
        error.context = std::move(context);
        error.priority = priority;

        _errors.emplace_back(std::move(error));
    }

    /** Renders the node as HILTI source code. */
    operator std::string() const { return print(); }

    virtual void dispatch(visitor::Dispatcher& v) = 0;

    Node& operator=(const Node& other) = delete;

protected:
    Node(Nodes children, Meta meta) : _children(std::move(children)), _meta(std::move(meta)) {
        for ( auto& c : _children ) {
            if ( c )
                c->_parent = this;
        }
    }

    Node(Meta meta) : _meta(std::move(meta)) {}
    Node(Node&& other) = default;
    Node(const Node& other) {
        _meta = other._meta;
        _prune_walk = other._prune_walk;
        _inherit_scope = other._inherit_scope;
        _children = other._children; // shallow copy by default; deepClone() will recreate this if needed
    }


    void clearChildren() {
        for ( auto& c : _children ) {
            if ( c )
                c->_clearParent();
        }

        _children.clear();
    }

    virtual bool isEqual(const Node& other) const {
        if ( _children.size() != other._children.size() )
            return false;

        for ( auto i = 0; i < _children.size(); ++i ) {
            if ( ! _children[i]->isEqual(*other._children[i]) )
                return false;
        }

        return true;
    }

    virtual NodePtr _clone(ASTContext* ctx) const = 0; // shallow copy
    virtual std::string _render() const { return ""; }

    friend bool operator==(const Node& x, const Node& y) { return x.isEqual(y); }

private:
    friend NodePtr node::clone(ASTContext* ctx, const NodePtr& n);
    friend NodePtr node::deepClone(ASTContext* ctx, const NodePtr& n);

    void _clearParent() { _parent = nullptr; }
    void _destroyChildrenRecursively(Node* n);

    Node& operator=(Node&& other) noexcept = default;

    Node* _parent = nullptr;
    Nodes _children;
    Meta _meta;

    bool _prune_walk = false;
    bool _inherit_scope = true;
    std::unique_ptr<Scope> _scope = nullptr;
    std::vector<node::Error> _errors;
};

namespace node {

/** Common mix-in class for nodes storing doc strings. */
class WithDocString {
public:
    /** Returns the documentation associated with the declaration, if any. */
    const std::optional<DocString>& documentation() const { return _doc; }

    /** Clears out any documentation associated with the declaration. */
    void clearDocumentation() { _doc.reset(); }

    /** Sets the documentation associated with the declaration. */
    void setDocumentation(DocString doc) {
        if ( doc )
            _doc = std::move(doc);
        else
            _doc.reset();
    }

private:
    std::optional<DocString> _doc;
};

/**
 * Creates `Node` instances for a vector of objects all implementing the
 * `Node` interface.
 */
template<typename T>
Nodes flatten(std::vector<T> t) {
    Nodes v;
    v.reserve(t.size());
    for ( const auto& i : t )
        v.emplace_back(std::move(i));

    return v;
}

/**
 * Copies a ramge of nodes over into a vector. Note that as with all copies of
 * node, this performs shallow copying.
 */
template<typename T>
Nodes flatten(hilti::node::Range<T> t) {
    Nodes v;
    v.reserve(t.size());
    for ( const auto& i : t )
        v.emplace_back(std::move(i));

    return v;
}

/** Create a 1-element vector of nodes for an object implementing the `Node` API. */
template<typename T>
inline Nodes flatten(NodePtr n) {
    return {std::move(n)};
}

template<typename T>
inline Nodes flatten(NodeDerivedPtr<T> n) {
    return {std::move(n)};
}

inline Nodes flatten() { return Nodes(); }

/**
 * Creates `Node` instances for objects all implementing the `Node`
 * interface.
 */
template<typename T, typename... Ts>
Nodes flatten(T t, Ts... ts) {
    return util::concat(std::move(flatten(t)), flatten(std::move(ts)...));
}

/**
 * Filters a node vector through a boolean predicate, returning a set
 * containing the matching ones.
 */
template<typename X, typename F>
auto filter(const hilti::node::Range<X>& x, F f) {
    hilti::node::Set<X> y;
    for ( const auto& i : x ) {
        if ( f(i) )
            y.push_back(i);
    }

    return y;
}

/**
 * Filters a node set through a boolean predicate, returning a new set
 * containing the matching ones.
 */
template<typename X, typename F>
auto filter(const hilti::node::Set<X>& x, F f) {
    hilti::node::Set<X> y;
    for ( const auto& i : x ) {
        if ( f(i) )
            y.insert(i);
    }

    return y;
}

/**
 * Applies a function to each element of a node range, returning a new vector
 * with the results.
 */
template<typename X, typename F>
auto transform(const hilti::node::Range<X>& x, F f) {
    using Y = std::invoke_result_t<F, NodeDerivedPtr<X>&>;
    std::vector<Y> y;
    y.reserve(x.size());
    for ( const auto& i : x )
        y.push_back(f(i));

    return y;
}

/**
 * Applies a function to each element of a node set, returning a new vector of
 * with the results.
 */
template<typename X, typename F>
auto transform(const hilti::node::Set<X>& x, F f) {
    using Y = std::invoke_result_t<F, NodeDerivedPtr<X>&>;
    std::vector<Y> y;
    y.reserve(x.size());
    for ( const auto& i : x )
        y.push_back(f(i));

    return y;
}

} // namespace node

/** Renders a node as HILTI source code. */
inline std::ostream& operator<<(std::ostream& out, const Node& n) {
    n.print(out, true);
    return out;
}

} // namespace hilti

inline hilti::node::Properties operator+(hilti::node::Properties p1, hilti::node::Properties p2) {
    p1.merge(p2);
    return p1;
}
