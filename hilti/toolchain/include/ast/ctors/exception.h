// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/ctor.h>
#include <hilti/ast/expression.h>
#include <hilti/ast/type.h>

#include "ast/forward.h"

namespace hilti::ctor {

/** AST node for a `exception` ctor. */
class Exception : public Ctor {
public:
    auto value() const { return child<Expression>(1); }

    QualifiedTypePtr type() const final { return child<QualifiedType>(0); }

    /** Constructs a exception value of a given type. */
    static auto create(ASTContext* ctx, const UnqualifiedTypePtr& type, const ExpressionPtr& value,
                       const Meta& meta = {}) {
        return NodeDerivedPtr<Exception>(new Exception({QualifiedType::create(ctx, type, true, meta), value}, meta));
    }

protected:
    Exception(Nodes children, Meta meta) : Ctor(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const override { return other.isA<Exception>() && Ctor::isEqual(other); }

    HILTI_NODE(Exception)
};

} // namespace hilti::ctor
