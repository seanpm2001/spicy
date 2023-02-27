// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/ctor.h>
#include <hilti/ast/types/union.h>

namespace hilti::ctor {

/** AST node for a `union` ctor. */
class Union : public Ctor {
public:
    /** Returns the value to initialize the unit with. */
    ExpressionPtr value() const { return child<Expression>(1); }

    QualifiedTypePtr type() const final { return child<QualifiedType>(0); }

    static auto create(ASTContext* ctx, const QualifiedTypePtr& type, const ExpressionPtr& value,
                       const Meta& meta = {}) {
        return NodeDerivedPtr<Union>(new Union({type, value}, meta));
    }

protected:
    Union(Nodes children, Meta meta) : Ctor(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const override { return other.isA<Union>() && Ctor::isEqual(other); }

    HILTI_NODE(Union)
};

} // namespace hilti::ctor
