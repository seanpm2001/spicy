// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/type.h>

namespace hilti::type {

/** AST node for an `auto` type. */
class Auto : public UnqualifiedType {
public:
protected:
    Auto(const Meta& meta) : UnqualifiedType(meta) {}

    bool _isResolved(ResolvedState* rstate) const final { return false; }

    bool isEqual(const Node& other) const override { return other.isA<Auto>() && UnqualifiedType::isEqual(other); }

    HILTI_NODE(Auto);

    // TODO: We limit instantiation to the corresponding QT's create() method
    // for now. Let's see if we need it somewhere else.
    friend class hilti::QualifiedType;
    static auto create(ASTContext* ctx, const Meta& m = Meta()) { return NodeDerivedPtr<Auto>(new Auto(m)); }
};

} // namespace hilti::type
