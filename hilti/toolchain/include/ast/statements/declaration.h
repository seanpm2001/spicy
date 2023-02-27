// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/declaration.h>
#include <hilti/ast/statement.h>

namespace hilti::statement {

/** AST node for a statement representing a declaration. */
class Declaration : public Statement {
public:
    auto declaration() const { return child<::hilti::Declaration>(0); }

    static auto create(ASTContext* ctx, const hilti::DeclarationPtr& d, Meta meta = {}) {
        return NodeDerivedPtr<Declaration>(new Declaration({d}, std::move(meta)));
    }

protected:
    Declaration(Nodes children, Meta meta) : Statement(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const final { return other.isA<Declaration>() && Statement::isEqual(other); }

    HILTI_NODE(Declaration)
};

} // namespace hilti::statement
