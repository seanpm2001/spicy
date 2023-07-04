// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <hilti/ast/declaration.h>
#include <hilti/ast/declarations/type.h>
#include <hilti/ast/node-range.h>
#include <hilti/ast/operator.h>
#include <hilti/ast/types/struct.h>
#include <hilti/ast/types/type.h>

#include "ast/detail/operator-registry.h"

namespace hilti::declaration {

class Function;

namespace function {

using Operator = hilti::FunctionCall;

} // namespace function

/** AST node for a function declaration. */
class Function : public Declaration {
public:
    ~Function() override {}

    auto function() const { return child<::hilti::Function>(0); }

    /** Returns an operator corresponding to a call to the function that the declaration corresponds to. */
    const auto& operator_() const { return _operator; }

    /**
     * If the parent declaration associated with the function refers to a valid
     * struct type, returns that type.
     */
    type::StructPtr parentStructType() const {
        assert(false && "TODO");
#if 0
        if ( ! _parent )
            return nullptr;

        return _parent->as<declaration::Type>()->type()->tryAs<type::Struct>();
#endif
    }

    void setFunction(const FunctionPtr& f) { setChild(0, f); }
    // void setParent(DeclarationPtr p) { _parent = std::move(p); }

    std::string displayName() const final { return "function"; }

    node::Properties properties() const final {
        auto p = node::Properties{{"operator", (_operator ? "set" : "not set")}};
        return Declaration::properties() + p;
    }

    static auto create(ASTContext* ctx, const FunctionPtr& function, declaration::Linkage linkage = Linkage::Private,
                       const Meta& meta = {}) {
        std::unique_ptr<function::Operator> op(new function::Operator());
        auto decl = NodeDerivedPtr<Function>(new Function({function}, op.get(), function->id(), linkage, meta));
        op->setDeclaration(decl.get());
        operator_::registry().register_(std::move(op));
        return decl;
    }

protected:
    Function(Nodes children, const Operator* op, ID id, declaration::Linkage linkage, Meta meta)
        : Declaration(std::move(children), std::move(id), linkage, std::move(meta)), _operator(op) {}

    bool isEqual(const Node& other) const override { return other.isA<Function>() && Declaration::isEqual(other); }

    HILTI_NODE(Function)

private:
    const Operator* _operator;
    // DeclarationPtr _parent = nullptr;
};

} // namespace hilti::declaration
