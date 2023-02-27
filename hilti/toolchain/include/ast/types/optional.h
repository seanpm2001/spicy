// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <utility>
#include <vector>

#include <hilti/ast/type.h>

namespace hilti::type {

/** AST node for an `optional<T>` type. */
class Optional : public UnqualifiedType {
public:
    QualifiedTypePtr dereferencedType() const final { return child(0)->as<QualifiedType>(); }
    Nodes typeParameters() const final { return children(); }

    static auto create(ASTContext* ctx, const QualifiedTypePtr& t, Meta m = Meta()) {
        return NodeDerivedPtr<Optional>(new Optional({t}, std::move(m)));
    }

    static auto create(ASTContext* ctx, Wildcard _, Meta m = Meta()) {
        return NodeDerivedPtr<Optional>(new Optional(Wildcard(), std::move(m)));
    }

protected:
    Optional(Nodes children, Meta meta) : UnqualifiedType(std::move(children), std::move(meta)) {}
    Optional(Wildcard _, Meta meta) : UnqualifiedType(Wildcard(), std::move(meta)) {}

    bool _isAllocable() const final { return true; }
    bool _isParameterized() const final { return true; }
    bool _isResolved(ResolvedState* rstate) const final { return type::detail::isResolved(dereferencedType(), rstate); }

    bool isEqual(const Node& other) const override { return other.isA<Optional>() && UnqualifiedType::isEqual(other); }

    HILTI_NODE(Optional)
};


} // namespace hilti::type
