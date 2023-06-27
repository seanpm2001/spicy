// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <hilti/ast/ctor.h>
#include <hilti/ast/types/regexp.h>

#include "ast/attribute.h"

namespace hilti::ctor {

/** AST node for a regular expression ctor. */
class RegExp : public Ctor {
public:
    const auto& value() const { return _value; }

    QualifiedTypePtr type() const final { return child<QualifiedType>(0); }
    auto attributes() const { return child<AttributeSet>(1); }

    /**
     * Returns true if this pattern does not need support for capturing groups.
     */
    bool isNoSub() const { return attributes()->find("&nosub")->hasValue(); }

    node::Properties properties() const final {
        auto p = node::Properties{{"value", util::join(_value, " | ")}};
        return Ctor::properties() + p;
    }

    static auto create(ASTContext* ctx, std::vector<std::string> v, const AttributeSetPtr& attrs,
                       const Meta& meta = {}) {
        return CtorPtr(
            new RegExp({QualifiedType::create(ctx, type::RegExp::create(ctx, meta), true), attrs}, std::move(v), meta));
    }

protected:
    RegExp(Nodes children, std::vector<std::string> v, Meta meta)
        : Ctor(std::move(children), std::move(meta)), _value(std::move(v)) {}

    bool isEqual(const Node& other) const override {
        auto n = other.tryAs<RegExp>();
        if ( ! n )
            return false;

        return Ctor::isEqual(other) && _value == n->_value;
    }

    HILTI_NODE(RegExp)

private:
    std::vector<std::string> _value;
};

} // namespace hilti::ctor
