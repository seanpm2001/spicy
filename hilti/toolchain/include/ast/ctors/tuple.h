// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/ctor.h>
#include <hilti/ast/expression.h>
#include <hilti/ast/types/tuple.h>

namespace hilti::ctor {

/** AST node for a tuple ctor. */
class Tuple : public Ctor {
public:
    QualifiedTypePtr type() const final { return child<QualifiedType>(0); }

    auto value() const { return children<Expression>(1, -1); }

    static auto create(ASTContext* ctx, const Expressions& exprs, const Meta& meta = {}) {
        auto type = _inferType(ctx, exprs, meta);
        return NodeDerivedPtr<Tuple>(new Tuple(node::flatten(type, exprs), meta));
    }

protected:
    Tuple(Nodes children, Meta meta) : Ctor(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const override { return other.isA<Tuple>() && Ctor::isEqual(other); }

    HILTI_NODE(Tuple)

private:
    static QualifiedTypePtr _inferType(ASTContext* ctx, const Expressions& exprs, const Meta& meta) {
        for ( const auto& e : exprs ) {
            if ( ! expression::isResolved(e) )
                return QualifiedType::createAuto(ctx, meta);
        }

        QualifiedTypes types;
        types.reserve(exprs.size());
        for ( const auto& e : exprs )
            types.emplace_back(e->type());

        return QualifiedType::create(ctx, type::Tuple::create(ctx, types, meta), true, meta);
    }
};
} // namespace hilti::ctor
