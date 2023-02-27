// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/ctor.h>
#include <hilti/ast/types/enum.h>

#include "ast/type.h"

namespace hilti::ctor {

/** AST node for a enum constructor. */
class Enum : public Ctor {
public:
    auto label() const { return child<type::enum_::Label>(0); }

    QualifiedTypePtr type() const final { return child<QualifiedType>(1); }

    static auto create(ASTContext* ctx, const type::enum_::LabelPtr& label, const Meta& meta = {}) {
        return NodeDerivedPtr<Enum>(
            new Enum({label, QualifiedType::create(ctx, label->enumType().lock(), true, meta)}, meta));
    }

protected:
    Enum(Nodes children, Meta meta) : Ctor(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const override { return other.isA<Enum>() && Ctor::isEqual(other); }

    HILTI_NODE(Enum)
};
} // namespace hilti::ctor
