// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <utility>

#include <hilti/ast/type.h>

namespace hilti::type {

/** AST node for a regular expression type. */
class RegExp : public UnqualifiedType {
public:
    static auto create(ASTContext* ctx, Meta meta = {}) { return NodeDerivedPtr<RegExp>(new RegExp(std::move(meta))); }

protected:
    RegExp(Meta meta) : UnqualifiedType(std::move(meta)) {}

    bool _isAllocable() const final { return true; }
    bool _isSortable() const final { return true; }

    bool isEqual(const Node& other) const override { return other.isA<RegExp>() && UnqualifiedType::isEqual(other); }

    HILTI_NODE(RegExp)
};

} // namespace hilti::type
