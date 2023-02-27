// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <utility>
#include <vector>

#include <hilti/ast/type.h>

namespace hilti::type {

/** AST node for an `result<T>` type. */
class Result : public UnqualifiedType {
public:
    QualifiedTypePtr dereferencedType() const final { return child(0)->as<QualifiedType>(); }
    Nodes typeParameters() const final { return children(); }

    static auto create(ASTContext* ctx, const QualifiedTypePtr& t, Meta m = Meta()) {
        return NodeDerivedPtr<Result>(new Result({t}, std::move(m)));
    }

    static auto create(ASTContext* ctx, Wildcard _, Meta m = Meta()) {
        return NodeDerivedPtr<Result>(new Result(Wildcard(), std::move(m)));
    }

protected:
    Result(Nodes children, Meta meta) : UnqualifiedType(std::move(children), std::move(meta)) {}
    Result(Wildcard _, Meta meta) : UnqualifiedType(Wildcard(), std::move(meta)) {}

    bool _isAllocable() const final { return true; }
    bool _isParameterized() const final { return true; }
    bool _isResolved(ResolvedState* rstate) const final { return type::detail::isResolved(dereferencedType(), rstate); }

    bool isEqual(const Node& other) const override { return other.isA<Result>() && UnqualifiedType::isEqual(other); }

    HILTI_NODE(Result)
};

} // namespace hilti::type
