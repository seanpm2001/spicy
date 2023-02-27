// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <hilti/ast/attribute.h>
#include <hilti/ast/declaration.h>
#include <hilti/ast/expression.h>
#include <hilti/ast/function.h>
#include <hilti/ast/id.h>
#include <hilti/ast/types/auto.h>

namespace hilti::declaration {

/** AST node for a struct/union field declaration. */
class Field : public Declaration {
public:
    auto callingConvention() const { return _cc; }
    auto attributes() const { return child<AttributeSet>(1); }
    auto inlineFunction() const { return child<hilti::Function>(2); }

    QualifiedTypePtr type() const {
        if ( const auto& func = inlineFunction() )
            return func->type();
        else
            return child<QualifiedType>(0);
    }

    bool isResolved(type::ResolvedState* rstate) const {
        if ( child(1)->isA<type::Function>() )
            return true;

        return type::detail::isResolved(type(), rstate);
    }

    ExpressionPtr default_() const {
        if ( auto a = attributes()->find("&default") )
            return *a->valueAsExpression();
        else
            return {};
    }

    auto isInternal() const { return attributes()->find("&internal") != nullptr; }
    auto isOptional() const { return attributes()->find("&optional") != nullptr; }
    auto isStatic() const { return attributes()->find("&static") != nullptr; }
    auto isNoEmit() const { return attributes()->find("&no-emit") != nullptr; }

    void setAttributes(const AttributeSetPtr& attrs) { setChild(2, attrs); }

    std::string displayName() const final { return "struct field"; }

    node::Properties properties() const final {
        auto p = node::Properties{{"cc", _cc ? to_string(*_cc) : "(not set)"}};
        return Declaration::properties() + p;
    }

    static auto create(ASTContext* ctx, ID id, const QualifiedTypePtr& type, const AttributeSetPtr& attrs,
                       Meta meta = {}) {
        return NodeDerivedPtr<Field>(new Field({type, attrs, nullptr}, std::move(id), {}, std::move(meta)));
    }

    static auto create(ASTContext* ctx, ID id, ::hilti::function::CallingConvention cc, const type::FunctionPtr& ftype,
                       const AttributeSetPtr& attrs, Meta meta = {}) {
        return NodeDerivedPtr<Field>(new Field({ftype, attrs, nullptr}, std::move(id), cc, std::move(meta)));
    }

    static auto create(ASTContext* ctx, const ID& id, const FunctionPtr& inline_func, const AttributeSetPtr& attrs,
                       Meta meta = {}) {
        return NodeDerivedPtr<Field>(new Field({nullptr, attrs, inline_func}, id, {}, std::move(meta)));
    }

protected:
    Field(Nodes children, ID id, std::optional<::hilti::function::CallingConvention> cc, Meta meta)
        : Declaration(std::move(children), std::move(id), declaration::Linkage::Struct, std::move(meta)) {}

    bool isEqual(const Node& other) const override {
        auto n = other.tryAs<Field>();
        if ( ! n )
            return false;

        return Declaration::isEqual(other) && _cc == n->_cc;
    }

    HILTI_NODE(Field)

private:
    std::optional<::hilti::function::CallingConvention> _cc;
};

using FieldPtr = std::shared_ptr<Field>;
using FieldList = std::vector<FieldPtr>;

} // namespace hilti::declaration
