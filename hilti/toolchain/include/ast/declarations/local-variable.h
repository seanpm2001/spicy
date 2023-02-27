// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <string>
#include <utility>

#include <hilti/ast/declaration.h>
#include <hilti/ast/declarations/expression.h>
#include <hilti/ast/expression.h>
#include <hilti/ast/type.h>

namespace hilti::declaration {

/** AST node for a local variable declaration. */
class LocalVariable : public Declaration {
public:
    auto init() const { return child<hilti::Expression>(1); }

    auto type() const {
        if ( auto t = child<QualifiedType>(0) )
            return t;
        else {
            assert(init());
            return init()->type();
        }
    }

    auto typeArguments() const { return children<hilti::Expression>(2, -1); }

    std::string displayName() const final { return "local variable"; }

    static auto create(ASTContext* ctx, ID id, QualifiedTypePtr type, Expressions args, ExpressionPtr init = nullptr,
                       Meta meta = {}) {
        return NodeDerivedPtr<LocalVariable>(
            new LocalVariable(node::flatten(std::move(type), std::move(init), std::move(args)), std::move(id),
                              std::move(meta)));
    }

    static auto create(ASTContext* ctx, ID id, QualifiedTypePtr type, ExpressionPtr init = nullptr, Meta meta = {}) {
        return create(ctx, std::move(id), std::move(type), {}, std::move(init), std::move(meta));
    }

    static auto create(ASTContext* ctx, ID id, QualifiedTypePtr type, Meta meta = {}) {
        return create(ctx, std::move(id), std::move(type), {}, nullptr, std::move(meta));
    }

    static auto create(ASTContext* ctx, ID id, ExpressionPtr init, Meta meta = {}) {
        return create(ctx, std::move(id), nullptr, {}, std::move(init), std::move(meta));
    }

    static auto create(ASTContext* ctx, ID id, Meta meta = {}) {
        return create(ctx, std::move(id), QualifiedType::createAuto(ctx, meta), std::move(meta));
    }

protected:
    LocalVariable(Nodes children, ID id, Meta meta)
        : Declaration(std::move(children), std::move(id), declaration::Linkage::Private, std::move(meta)) {}

    bool isEqual(const Node& other) const override { return other.isA<LocalVariable>() && Declaration::isEqual(other); }

    HILTI_NODE(LocalVariable)
};

} // namespace hilti::declaration
