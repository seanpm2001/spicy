// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/type.h>

namespace hilti::type {

/** AST node for an `any` type. */
class Any : public UnqualifiedType {
public:
    static auto create(ASTContext* ctx, Meta m = Meta()) { return NodeDerivedPtr<Any>(new Any(std::move(m))); }

protected:
    Any(Meta meta) : UnqualifiedType(std::move(meta)) {}

    bool _isAllocable() const override { return true; }
    bool _isSortable() const override { return true; }

    bool isEqual(const Node& other) const override { return other.isA<Any>() && UnqualifiedType::isEqual(other); }

    HILTI_NODE(Any);
};

} // namespace hilti::type
