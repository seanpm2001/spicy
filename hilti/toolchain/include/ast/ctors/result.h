// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/ctor.h>
#include <hilti/ast/expression.h>
#include <hilti/ast/type.h>
#include <hilti/ast/types/error.h>
#include <hilti/ast/types/result.h>

namespace hilti::ctor {

/** AST node for a `optional` ctor. */
class Result : public Ctor {
public:
    QualifiedTypePtr type() const final {
        if ( auto e = child(0) )
            return child<QualifiedType>(0);
        else
            return child<Expression>(1)->type();
    }

    QualifiedTypePtr dereferencedType() const { return type()->type()->as<type::Result>()->dereferencedType(); }

    ExpressionPtr value() const {
        const auto& e = child<Expression>(1);

        if ( ! e->type()->isA<type::Error>() )
            return e;
        else
            return {};
    }

    ExpressionPtr error() const {
        const auto& e = child<Expression>(1);

        if ( e->type()->isA<type::Error>() )
            return e;
        else
            return {};
    }

    static auto create(ASTContext* ctx, const ExpressionPtr& expr, const Meta& meta = {}) {
        return NodeDerivedPtr<Result>(new Result(
            {
                nullptr,
                expr,
            },
            meta));
    }

protected:
    Result(Nodes children, Meta meta) : Ctor(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const override { return other.isA<Result>() && Ctor::isEqual(other); }

    HILTI_NODE(Result)
};

} // namespace hilti::ctor
