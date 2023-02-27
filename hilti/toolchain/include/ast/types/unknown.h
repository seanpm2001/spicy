// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <utility>

#include <hilti/ast/type.h>

namespace hilti::type {

/** AST node for an unknown place-holder type. */
class Unknown : public UnqualifiedType {
public:
    static auto create(ASTContext* ctx, Meta meta = {}) {
        return NodeDerivedPtr<Unknown>(new Unknown(std::move(meta)));
    }

protected:
    Unknown(Meta meta) : UnqualifiedType(std::move(meta)) {}

    bool _isAllocable() const final { return true; }

    bool isEqual(const Node& other) const override { return other.isA<Unknown>() && UnqualifiedType::isEqual(other); }

    HILTI_NODE(Unknown)
};

} // namespace hilti::type
