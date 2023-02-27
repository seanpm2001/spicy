// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/type.h>

namespace hilti::type {

/** AST node for a `bool` type. */
class Bool : public UnqualifiedType {
public:
    static auto create(ASTContext* ctx, Meta meta = {}) { return NodeDerivedPtr<Bool>(new Bool(std::move(meta))); }

protected:
    Bool(Meta meta) : UnqualifiedType(std::move(meta)) {}

    bool _isAllocable() const final { return true; }
    bool _isSortable() const final { return true; }

    bool isEqual(const Node& other) const override { return other.isA<Bool>() && UnqualifiedType::isEqual(other); }

    HILTI_NODE(Bool)
};

} // namespace hilti::type
