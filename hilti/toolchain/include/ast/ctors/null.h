// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/ctor.h>
#include <hilti/ast/types/null.h>

namespace hilti::ctor {

/** AST node for a `Null` ctor. */
class Null : public Ctor {
public:
    QualifiedTypePtr type() const final { return child<QualifiedType>(0); }

    static auto create(ASTContext* ctx, const Meta& meta = {}) {
        return NodeDerivedPtr<Null>(new Null({QualifiedType::create(ctx, type::Null::create(ctx, meta), true)}, meta));
    }

protected:
    Null(Nodes children, Meta meta) : Ctor(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const override { return other.isA<Null>() && Ctor::isEqual(other); }

    HILTI_NODE(Null)
};

} // namespace hilti::ctor
