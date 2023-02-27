// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/ctor.h>

namespace hilti::ctor {

/** AST node for a constructor that's been coerced from one type to another. */
class Coerced : public Ctor {
public:
    auto originalCtor() const { return child<Ctor>(0); }
    auto coercedCtor() const { return child<Ctor>(1); }

    QualifiedTypePtr type() const final { return coercedCtor()->type(); }
    bool isLhs() const final { return coercedCtor()->isLhs(); }
    bool isTemporary() const final { return coercedCtor()->isTemporary(); }

    static auto create(ASTContext* ctx, const CtorPtr& orig, const CtorPtr& new_, const Meta& meta = {}) {
        return NodeDerivedPtr<Coerced>(new Coerced({orig, new_}, meta));
    }

protected:
    Coerced(Nodes children, Meta meta) : Ctor(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const override { return other.isA<Coerced>() && Ctor::isEqual(other); }

    HILTI_NODE(Coerced)
};

} // namespace hilti::ctor
