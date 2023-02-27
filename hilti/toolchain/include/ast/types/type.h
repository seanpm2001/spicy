// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/type.h>

namespace hilti::type {

/** AST node for a type representing a type value. */
class Type_ : public UnqualifiedType {
public:
    auto typeValue() const { return child<QualifiedType>(0); }

    Nodes typeParameters() const final { return children(); }

    static auto create(ASTContext* ctx, const QualifiedTypePtr& type, Meta meta = {}) {
        return NodeDerivedPtr<Type_>(new Type_({type}, std::move(meta)));
    }

    static auto create(ASTContext* ctx, Wildcard _, Meta m = Meta()) {
        return NodeDerivedPtr<Type_>(new Type_(Wildcard(), std::move(m)));
    }

protected:
    Type_(Nodes children, Meta meta) : UnqualifiedType(std::move(children), std::move(meta)) {}
    Type_(Wildcard _, Meta meta) : UnqualifiedType(Wildcard(), std::move(meta)) {}

    bool _isParameterized() const final { return true; }
    bool _isResolved(ResolvedState* rstate) const final { return type::detail::isResolved(typeValue(), rstate); }

    bool isEqual(const Node& other) const override { return other.isA<Type_>() && UnqualifiedType::isEqual(other); }

    HILTI_NODE(Type_)
};

} // namespace hilti::type
