// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <hilti/ast/declaration.h>
#include <hilti/ast/expression.h>

namespace hilti::declaration {

/** AST node for a property declaration. */
class Property : public Declaration {
public:
    auto expression() const { return children()[0]->tryAs<::hilti::Expression>(); }

    std::string displayName() const final { return "property"; }

    static auto create(ASTContext* ctx, ID id, Meta meta = {}) {
        return NodeDerivedPtr<Property>(new Property({}, std::move(id), std::move(meta)));
    }

    static auto create(ASTContext* ctx, ID id, const ExpressionPtr& expr, Meta meta = {}) {
        return NodeDerivedPtr<Property>(new Property({expr}, std::move(id), std::move(meta)));
    }

protected:
    Property(Nodes children, ID id, Meta meta)
        : Declaration(std::move(children), std::move(id), Linkage::Private, std::move(meta)) {}

    bool isEqual(const Node& other) const override { return other.isA<Property>() && Declaration::isEqual(other); }

    HILTI_NODE(Property)
};

using PropertyPtr = std::shared_ptr<Property>;
using Properties = std::vector<Property>;

} // namespace hilti::declaration
