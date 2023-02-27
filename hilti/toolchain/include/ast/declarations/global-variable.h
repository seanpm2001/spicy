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

/** AST node for a global variable declaration. */
class GlobalVariable : public Declaration {
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

    std::string displayName() const final { return "global variable"; }

    static auto create(ASTContext* ctx, ID id, QualifiedTypePtr type, Expressions args, ExpressionPtr init = nullptr,
                       declaration::Linkage linkage = Linkage::Private, Meta meta = {}) {
        return NodeDerivedPtr<GlobalVariable>(
            new GlobalVariable(node::flatten(std::move(type), std::move(init), std::move(args)), std::move(id), linkage,
                               std::move(meta)));
    }

    static auto create(ASTContext* ctx, ID id, QualifiedTypePtr type, ExpressionPtr init = nullptr,
                       declaration::Linkage linkage = Linkage::Private, Meta meta = {}) {
        return create(ctx, std::move(id), std::move(type), {}, std::move(init), linkage, std::move(meta));
    }

    static auto create(ASTContext* ctx, ID id, QualifiedTypePtr type, declaration::Linkage linkage = Linkage::Private,
                       Meta meta = {}) {
        return create(ctx, std::move(id), std::move(type), {}, nullptr, linkage, std::move(meta));
    }

    static auto create(ASTContext* ctx, ID id, ExpressionPtr init, declaration::Linkage linkage = Linkage::Private,
                       Meta meta = {}) {
        return create(ctx, std::move(id), nullptr, {}, std::move(init), linkage, std::move(meta));
    }

    static auto create(ASTContext* ctx, ID id, declaration::Linkage linkage = Linkage::Private, Meta meta = {}) {
        return create(ctx, std::move(id), QualifiedType::createAuto(ctx, meta), linkage, std::move(meta));
    }

protected:
    GlobalVariable(Nodes children, ID id, declaration::Linkage linkage, Meta meta)
        : Declaration(std::move(children), std::move(id), linkage, std::move(meta)) {}

    bool isEqual(const Node& other) const override {
        return other.isA<GlobalVariable>() && Declaration::isEqual(other);
    }

    HILTI_NODE(GlobalVariable)
};

} // namespace hilti::declaration
