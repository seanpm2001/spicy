// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/ctor.h>
#include <hilti/ast/expression.h>
#include <hilti/ast/types/bool.h>
#include <hilti/ast/types/set.h>

namespace hilti::ctor {

/** AST node for a `set` ctor. */
class Set : public Ctor {
public:
    auto elementType() const { return type()->type()->as<type::Set>()->elementType(); }
    auto value() const { return children<Expression>(1, -1); }

    QualifiedTypePtr type() const final { return child<QualifiedType>(0); }

    static auto create(ASTContext* ctx, const QualifiedTypePtr& etype, Expressions exprs, const Meta& meta = {}) {
        auto stype = QualifiedType::create(ctx, type::Set::create(ctx, etype, meta), true, meta);
        return NodeDerivedPtr<Set>(new Set(node::flatten(stype, std::move(exprs)), meta));
    }

    static auto create(ASTContext* ctx, Expressions exprs, const Meta& meta = {}) {
        // bool is just an arbitrary place-holder type for empty values.
        auto etype = exprs.empty() ? QualifiedType::create(ctx, type::Bool::create(ctx, meta), true, meta) :
                                     QualifiedType::createAuto(ctx, meta);
        return create(ctx, etype, std::move(exprs), meta);
    }

protected:
    Set(Nodes children, Meta meta) : Ctor(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const override { return other.isA<Set>() && Ctor::isEqual(other); }

    HILTI_NODE(Set)
};

} // namespace hilti::ctor
