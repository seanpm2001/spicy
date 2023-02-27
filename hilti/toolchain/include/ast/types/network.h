// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <utility>

#include <hilti/ast/type.h>

namespace hilti::type {

/** AST node for a `network` type. */
class Network : public UnqualifiedType {
public:
    static auto create(ASTContext* ctx, Meta meta = {}) {
        return NodeDerivedPtr<Network>(new Network(std::move(meta)));
    }

protected:
    Network(Meta meta) : UnqualifiedType(std::move(meta)) {}

    bool _isAllocable() const final { return true; }
    bool _isSortable() const final { return true; }

    bool isEqual(const Node& other) const override { return other.isA<Network>() && UnqualifiedType::isEqual(other); }

    HILTI_NODE(Network)
};

} // namespace hilti::type
