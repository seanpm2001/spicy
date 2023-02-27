// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/type.h>

#include "ast/type.h"

namespace hilti::type {

/** AST node for a `exception` type. */
class Exception : public UnqualifiedType {
public:
    auto baseType() const { return child<UnqualifiedType>(0); }

    Nodes typeParameters() const final { return children(); }

    static auto create(ASTContext* ctx, const UnqualifiedTypePtr& base, Meta meta = {}) {
        return NodeDerivedPtr<Exception>(new Exception({base}, std::move(meta)));
    }

    static auto create(ASTContext* ctx, const Meta& meta = {}) { return create(ctx, nullptr, meta); }

    static auto create(ASTContext* ctx, Wildcard _, Meta m = Meta()) {
        return NodeDerivedPtr<Exception>(new Exception(Wildcard(), std::move(m)));
    }

protected:
    Exception(Nodes children, Meta meta) : UnqualifiedType(std::move(children), std::move(meta)) {}
    Exception(Wildcard _, Meta meta) : UnqualifiedType(Wildcard(), std::move(meta)) {}

    bool _isAllocable() const final { return true; }
    bool _isParameterized() const final { return true; }

    bool _isResolved(ResolvedState* rstate) const final {
        return baseType() ? type::detail::isResolved(baseType(), rstate) : true;
    }

    bool isEqual(const Node& other) const override { return other.isA<Exception>() && UnqualifiedType::isEqual(other); }

    HILTI_NODE(Exception)
};

} // namespace hilti::type
