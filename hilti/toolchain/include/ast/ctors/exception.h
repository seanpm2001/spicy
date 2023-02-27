// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/ctor.h>
#include <hilti/ast/expression.h>
#include <hilti/ast/type.h>

namespace hilti::ctor {

/** AST node for a `exception` ctor. */
class Exception : public Ctor {
public:
    QualifiedTypePtr type() const final { return child<QualifiedType>(0); }
    auto typeArguments() const { return children<Expression>(1, -1); }

    /** Constructs a exception value of a given type. */
    static auto create(ASTContext* ctx, const UnqualifiedTypePtr& type, const Meta& meta = {}) {
        return NodeDerivedPtr<Exception>(new Exception({QualifiedType::create(ctx, type, true, meta)}, meta));
    }

    /**
     * Constructs a exception value of a given type, passing specified arguments to
     * types with parameters.
     */
    static auto create(ASTContext* ctx, const UnqualifiedTypePtr& type, Expressions type_args, const Meta& meta = {}) {
        return CtorPtr(
            new Exception(node::flatten(QualifiedType::create(ctx, type, true, meta), std::move(type_args)), meta));
    }

protected:
    Exception(Nodes children, Meta meta) : Ctor(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const override { return other.isA<Exception>() && Ctor::isEqual(other); }

    HILTI_NODE(Exception)
};

} // namespace hilti::ctor
