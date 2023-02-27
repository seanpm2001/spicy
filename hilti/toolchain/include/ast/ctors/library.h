// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/ctor.h>
#include <hilti/ast/expression.h>
#include <hilti/ast/type.h>
#include <hilti/ast/types/library.h>

namespace hilti::ctor {

/**
 * AST node for a constructor of an instance of a library type. Because we
 * don't know more about the internal representation of the library type, we
 * represent the value through a ctor of another, known type. The code
 * generator must ensure that coercion operates correctly for the final C++
 * code.
 **/
class Library : public Ctor {
public:
    ExpressionPtr value() const { return child<Expression>(0); }
    QualifiedTypePtr type() const final { return child<QualifiedType>(1); }

    static auto create(ASTContext* ctx, const CtorPtr& ctor, const QualifiedTypePtr& type, const Meta& meta = {}) {
        return NodeDerivedPtr<Library>(new Library(
            {
                ctor,
                type,
            },
            meta));
    }

protected:
    Library(Nodes children, Meta meta) : Ctor(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const override { return other.isA<Library>() && Ctor::isEqual(other); }

    HILTI_NODE(Library)
};

} // namespace hilti::ctor
