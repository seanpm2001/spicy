// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <unordered_set>
#include <utility>
#include <vector>

#include <hilti/ast/id.h>
#include <hilti/ast/node.h>
#include <hilti/ast/visitor.h>

namespace hilti {

namespace builder {
class NodeBuilder;
}

namespace declaration {
class Parameter;
}

namespace type::function {
using Parameter = declaration::Parameter;
} // namespace type::function

namespace type {

using ResolvedState = std::unordered_set<uintptr_t>;

struct Wildcard {
    explicit Wildcard() = default;
};

} // namespace type

/** * Base class for classes implementing types. */
class UnqualifiedType : public Node {
public:
    ~UnqualifiedType() override;

    const auto& cxxID() const { return _cxx_id; }
    void setCxxID(const ID& id) { _cxx_id = id.makeAbsolute(); }

    const auto& typeID() const { return _type_id; }
    void setTypeID(ID id) { _type_id = std::move(id); }

    const auto& resolvedID() const { return _resolved_id; }
    void setResolvedID(ID id) { _resolved_id = std::move(id); }

    /**
     * Returns true if all instances of the same type class can be coerced into
     * the current instance, independent of their parameters. In HILTI source
     * code, this typically corresponds to a type `T<*>`. Relevant only for
     * types with type parameters.
     */
    bool isWildcard() const { return _is_wildcard; }

    /** Returns the type of elements the iterator traverse. */
    virtual QualifiedTypePtr dereferencedType() const { return {}; }


    /** Returns the type of elements the container stores. */
    virtual QualifiedTypePtr elementType() const { return {}; }

    /** Returns the type of an iterator for this type. */
    virtual UnqualifiedTypePtr iteratorType() const { return {}; }

    /** Returns any parameters the type expects. */
    virtual hilti::node::Set<type::function::Parameter> parameters() const { return {}; }

    /**
     * Returns any parameters associated with type. If a type is declared as
     * `T<A,B,C>` this returns a vector of the AST nodes for `A`, `B`, and
     * `C`.
     */
    virtual Nodes typeParameters() const { return {}; } // TODO: Is this really needed? Seems fuzzy.

    /** Returns the type of an view for this type. */
    virtual UnqualifiedTypePtr viewType() const { return {}; }

    /** For internal use. Use ``type::isAllocable` instead. */
    virtual bool _isAllocable() const { return false; }

    /** For internal use. Use ``type::isIterator` instead. */
    virtual bool _isIterator() const { return false; }

    /** For internal use. Use ``type::isMutable` instead. */
    virtual bool _isMutable() const { return false; }

    /** For internal use. Use ``type::isParameterized` instead. */
    virtual bool _isParameterized() const { return false; }

    /** For internal use. Use ``type::isReferenceType` instead. */
    virtual bool _isReferenceType() const { return false; }

    /** For internal use. Use ``type::isResolved` instead. */
    virtual bool _isResolved(type::ResolvedState* rstate) const { return true; }

    /** For internal use. Use ``type::isRuntimeNonTrivial` instead. */
    virtual bool _isRuntimeNonTrivial() const { return false; }

    /** For internal use. Use ``type::isSortable` instead. */
    virtual bool _isSortable() const { return false; }

    hilti::node::Properties properties() const override {
        auto p = node::Properties{{
            {"cxx-id", _cxx_id},
            {"id", _type_id},
            {"resolved-id", _resolved_id},
        }};

        return Node::properties() + p;
    }

protected:
    UnqualifiedType(Meta meta) : Node::Node(std::move(meta)) {}
    UnqualifiedType(Nodes children, Meta meta) : Node::Node(std::move(children), std::move(meta)) {}
    UnqualifiedType(type::Wildcard _, Meta meta) : Node::Node({}, std::move(meta)), _is_wildcard(true) {}

    std::string _render() const override;

    bool isEqual(const Node& other) const override {
        auto n = other.tryAs<UnqualifiedType>();
        if ( ! n )
            return false;

        if ( _type_id && n->_type_id )
            return *_type_id == *n->_type_id;

        if ( _cxx_id && n->_cxx_id )
            return *_cxx_id == *n->_cxx_id;

        return Node::isEqual(other) && _type_id == n->_type_id && _cxx_id == n->_cxx_id &&
               _resolved_id == n->_resolved_id;
    }

    HILTI_NODE_BASE(Type);

private:
    bool _is_wildcard = false;
    std::optional<ID> _type_id;
    std::optional<ID> _cxx_id;
    std::optional<ID> _resolved_id;
};

// Type with associated constness.
class QualifiedType : public Node {
public:
    hilti::node::Properties properties() const override { return {{"const", _is_constant}, {"auto", _is_auto}}; }

    UnqualifiedTypePtr type() const { return child<UnqualifiedType>(0); }
    bool isConstant() const { return _is_constant; }
    bool isAuto() const { return _is_auto; }

    static auto create(ASTContext* ctx, const UnqualifiedTypePtr& t, bool is_constant, Meta m = Meta()) {
        // Note: We allow (i.e., must support) `ctx` being null.
        return NodeDerivedPtr<QualifiedType>(new QualifiedType(Nodes{t}, is_constant, false, std::move(m)));
    }

    static QualifiedTypePtr createAuto(ASTContext* ctx, const Meta& m = Meta());

protected:
    QualifiedType(Nodes children, bool is_constant, bool is_auto, Meta meta)
        : Node(std::move(children), std::move(meta)), _is_constant(is_constant), _is_auto(is_auto) {}

    std::string _render() const final;
    bool isEqual(const Node& other) const override {
        auto n = other.tryAs<QualifiedType>();
        if ( ! n )
            return false;

        return Node::isEqual(other) && _is_constant == n->_is_constant && _is_auto == n->_is_auto;
    }

    HILTI_NODE(QualifiedType);

private:
    bool _is_constant = false;
    bool _is_auto = false;
};

namespace type {

/** Returns true for HILTI types that can be used to instantiate variables. */
inline bool isAllocable(const UnqualifiedTypePtr& t) { return t->_isAllocable(); }
inline bool isAllocable(const QualifiedTypePtr& t) { return t->type()->_isAllocable(); }

/** Returns true for HILTI types that can be compared for ordering at runtime. */
inline bool isSortable(const UnqualifiedTypePtr& t) { return t->_isSortable(); }
inline bool isSortable(const QualifiedTypePtr& t) { return t->type()->_isSortable(); }

/** Returns true for HILTI types that one can iterator over. */
inline bool isIterable(const UnqualifiedTypePtr& t) { return t->iteratorType().get(); }
inline bool isIterable(const QualifiedTypePtr& t) { return t->type()->iteratorType().get(); }

/** Returns true for HILTI types that represent iterators. */
inline bool isIterator(const UnqualifiedTypePtr& t) { return t->_isIterator(); }
inline bool isIterator(const QualifiedTypePtr& t) { return t->type()->_isIterator(); }

/** Returns true for HILTI types that are parameterized with a set of type parameters. */
inline bool isParameterized(const UnqualifiedTypePtr& t) { return t->_isParameterized(); }
inline bool isParameterized(const QualifiedTypePtr& t) { return t->type()->_isParameterized(); }

/** Returns true for HILTI types that implement a reference to another type. */
inline bool isReferenceType(const UnqualifiedTypePtr& t) { return t->_isReferenceType(); }
inline bool isReferenceType(const QualifiedTypePtr& t) { return t->type()->_isReferenceType(); }

/** Returns true for HILTI types that can change their value. */
inline bool isMutable(const UnqualifiedTypePtr& t) { return t->_isMutable(); }
inline bool isMutable(const QualifiedTypePtr& t) { return t->type()->_isMutable(); }
inline bool isMutable(const UnqualifiedType& t) { return t._isMutable(); }
inline bool isMutable(const QualifiedType& t) { return t.type()->_isMutable(); }

/** Returns true for HILTI types that, when compiled, correspond to non-POD C++ types. */
inline bool isRuntimeNonTrivial(const UnqualifiedTypePtr& t) { return t->_isRuntimeNonTrivial(); }
inline bool isRuntimeNonTrivial(const QualifiedTypePtr& t) { return t->type()->_isRuntimeNonTrivial(); }

/** Returns true for HILTI types that one can create a view for. */
inline bool isViewable(const UnqualifiedTypePtr& t) { return t->viewType().get(); }
inline bool isViewable(const QualifiedTypePtr& t) { return t->type()->viewType().get(); }

/** Returns true for HILTI types that may receive type arguments on instantiations. */
inline bool takesArguments(const UnqualifiedTypePtr& t) { return ! t->parameters().empty(); }
inline bool takesArguments(const QualifiedTypePtr& t) { return ! t->type()->parameters().empty(); }

namespace detail {
// Internal backend for the `isResolved()`.
extern bool isResolved(const hilti::UnqualifiedType& t, ResolvedState* rstate);
inline bool isResolved(const hilti::QualifiedType& t, ResolvedState* rstate) { return isResolved(*(t.type()), rstate); }
inline extern bool isResolved(const hilti::UnqualifiedTypePtr& t, ResolvedState* rstate) {
    return isResolved(*t, rstate);
}
inline extern bool isResolved(const hilti::QualifiedTypePtr& t, ResolvedState* rstate) {
    return isResolved(t->type(), rstate);
}
} // namespace detail

/** Returns true if the type has been fully resolved, including all sub-types it may include. */
extern bool isResolved(const UnqualifiedType& t);
inline bool isResolved(const UnqualifiedTypePtr& t) { return isResolved(*t); }
inline bool isResolved(const QualifiedType& t) { return isResolved(t.type()); }
inline bool isResolved(const QualifiedTypePtr& t) { return isResolved(t->type()); }

namespace detail {
// Internal backends for the `pruneWalk()`.
extern void applyPruneWalk(hilti::UnqualifiedType* t);
} // namespace detail

inline void pruneWalk(const UnqualifiedTypePtr& t) { detail::applyPruneWalk(t.get()); }
inline void pruneWalk(const QualifiedTypePtr& t) { detail::applyPruneWalk(t->type().get()); }

} // namespace type
} // namespace hilti
