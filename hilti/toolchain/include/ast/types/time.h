// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <utility>

#include <hilti/ast/type.h>

namespace hilti::type {

/** AST node for a `time` type. */
class Time : public UnqualifiedType {
public:
    static auto create(ASTContext* ctx, Meta meta = {}) { return NodeDerivedPtr<Time>(new Time(std::move(meta))); }

protected:
    Time(Meta meta) : UnqualifiedType(std::move(meta)) {}

    bool _isAllocable() const final { return true; }
    bool _isSortable() const final { return true; }

    bool isEqual(const Node& other) const override { return other.isA<Time>() && UnqualifiedType::isEqual(other); }

    HILTI_NODE(Time)
};

} // namespace hilti::type
