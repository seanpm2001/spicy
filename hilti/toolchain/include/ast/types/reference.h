// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <utility>
#include <vector>

#include <hilti/ast/type.h>
#include <hilti/ast/types/unknown.h>
#include <hilti/base/optional-ref.h>

namespace hilti::type {

/*
 * AST node for a `strong_ref<T>` type.
 */
class StrongReference : public TypeBase {
public:
    StrongReference(Wildcard /*unused*/, Meta m = Meta()) : TypeBase({type::unknown}, std::move(m)), _wildcard(true) {}
    StrongReference(Type ct, Meta m = Meta()) : TypeBase(nodes(std::move(ct)), std::move(m)) {}
    StrongReference(NodeRef ct, Meta m = Meta()) : TypeBase(nodes(node::none), std::move(m)), _type(std::move(ct)) {}

    optional_ref<const Type> dereferencedType() const override {
        if ( _type )
            return _type->as<Type>();
        else
            return children()[0].as<Type>();
    }

    bool operator==(const StrongReference& other) const { return dereferencedType() == other.dereferencedType(); }

    bool isEqual(const Type& other) const override { return node::isEqual(this, other); }

    bool _isResolved(ResolvedState* rstate) const override {
        return type::detail::isResolved(dereferencedType(), rstate);
    }

    std::vector<Node> typeParameters() const override { return children(); }
    bool isWildcard() const override { return _wildcard; }

    node::Properties properties() const override { return node::Properties{{"type", _type.renderedRid()}}; }

    bool _isAllocable() const override { return true; }
    bool _isParameterized() const override { return true; }
    bool _isReferenceType() const override { return true; }

    const std::type_info& typeid_() const override { return typeid(decltype(*this)); }

    HILTI_TYPE_VISITOR_IMPLEMENT

private:
    bool _wildcard = false;
    NodeRef _type;
};

/** AST node for a `weak_ref<T>` type. */
class WeakReference : public TypeBase {
public:
    WeakReference(Wildcard /*unused*/, Meta m = Meta()) : TypeBase({type::unknown}, std::move(m)), _wildcard(true) {}
    WeakReference(Type ct, Meta m = Meta()) : TypeBase({std::move(ct)}, std::move(m)) {}

    optional_ref<const Type> dereferencedType() const override { return children()[0].as<Type>(); }

    bool operator==(const WeakReference& other) const { return dereferencedType() == other.dereferencedType(); }

    bool isEqual(const Type& other) const override { return node::isEqual(this, other); }

    bool _isResolved(ResolvedState* rstate) const override {
        return type::detail::isResolved(dereferencedType(), rstate);
    }

    std::vector<Node> typeParameters() const override { return children(); }
    bool isWildcard() const override { return _wildcard; }

    node::Properties properties() const override { return node::Properties{}; }

    bool _isAllocable() const override { return true; }
    bool _isParameterized() const override { return true; }
    bool _isReferenceType() const override { return true; }

    const std::type_info& typeid_() const override { return typeid(decltype(*this)); }

    HILTI_TYPE_VISITOR_IMPLEMENT

private:
    bool _wildcard = false;
};

/** AST node for a `val_ref<T>` type. */
class ValueReference : public TypeBase {
public:
    ValueReference(Wildcard /*unused*/, Meta m = Meta())
        : TypeBase(nodes(type::unknown), std::move(m)), _wildcard(true) {}
    ValueReference(Type ct, Meta m = Meta()) : TypeBase(nodes(std::move(ct)), std::move(m)) {}
    ValueReference(NodeRef ct, Meta m = Meta()) : TypeBase(nodes(type::unknown), std::move(m)), _node(std::move(ct)) {}

    optional_ref<const Type> dereferencedType() const override {
        if ( _node )
            return _node->as<Type>();
        else
            return children()[0].as<Type>();
    }

    bool operator==(const ValueReference& other) const { return dereferencedType() == other.dereferencedType(); }

    bool isEqual(const Type& other) const override { return node::isEqual(this, other); }

    bool _isResolved(ResolvedState* rstate) const override {
        return type::detail::isResolved(dereferencedType(), rstate);
    }

    std::vector<Node> typeParameters() const override { return children(); }
    bool isWildcard() const override { return _wildcard; }

    node::Properties properties() const override { return node::Properties{{"rid", (_node ? _node->rid() : 0U)}}; }

    bool _isAllocable() const override { return true; }
    bool _isParameterized() const override { return true; }
    bool _isReferenceType() const override { return true; }

    const std::type_info& typeid_() const override { return typeid(decltype(*this)); }

    HILTI_TYPE_VISITOR_IMPLEMENT

private:
    bool _wildcard = false;
    NodeRef _node;
};

} // namespace hilti::type
