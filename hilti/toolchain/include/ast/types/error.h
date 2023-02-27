// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/type.h>

namespace hilti::type {

/** AST node for a error type. */
class Error : public UnqualifiedType {
public:
    static auto create(ASTContext* ctx, Meta meta = {}) { return NodeDerivedPtr<Error>(new Error(std::move(meta))); }

protected:
    Error(Meta meta) : UnqualifiedType(std::move(meta)) {}

    bool _isAllocable() const final { return true; }

    bool isEqual(const Node& other) const override { return other.isA<Error>() && UnqualifiedType::isEqual(other); }

    HILTI_NODE(Error)
};

} // namespace hilti::type
