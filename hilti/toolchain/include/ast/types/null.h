// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <utility>

#include <hilti/ast/type.h>

namespace hilti::type {

/**
 * AST node for a null type.
 *
 * TODO: Do we still need this?
 */
class Null : public UnqualifiedType {
public:
    static auto create(ASTContext* ctx, Meta meta = {}) { return NodeDerivedPtr<Null>(new Null(std::move(meta))); }

protected:
    Null(Meta meta) : UnqualifiedType(std::move(meta)) {}

    bool isEqual(const Node& other) const override { return other.isA<Null>() && UnqualifiedType::isEqual(other); }

    HILTI_NODE(Null)
};

} // namespace hilti::type
