// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <string>
#include <utility>

#include <hilti/ast/declaration.h>
#include <hilti/ast/declarations/type.h>
#include <hilti/ast/function.h>
#include <hilti/ast/types/struct.h>

namespace hilti::declaration {

/** AST node for a function declaration. */
class Function : public Declaration {
public:
    FunctionPtr function() const { return child<::hilti::Function>(0); }

    /**
     * Returns the parent declaration associated with the function, if any. For
     * methods, this will the declaration of the corresponding struct type.
     */
    DeclarationPtr parent() const { return _parent; }

    /**
     * If the parent declaration associated with the function refers to a valid
     * struct type, returns that type.
     */
    type::StructPtr parentStructType() const {
        if ( ! _parent )
            return nullptr;

        return _parent->as<declaration::Type>()->type()->tryAs<type::Struct>();
    }

    void setFunction(const FunctionPtr& f) { setChild(0, f); }
    void setParent(DeclarationPtr p) { _parent = std::move(p); }

    std::string displayName() const final { return "function"; }

    static auto create(ASTContext* ctx, const FunctionPtr& function, declaration::Linkage linkage = Linkage::Private,
                       Meta meta = {}) {
        return NodeDerivedPtr<Function>(new Function({function}, function->id(), linkage, std::move(meta)));
    }

protected:
    Function(Nodes children, ID id, declaration::Linkage linkage, Meta meta)
        : Declaration(std::move(children), std::move(id), linkage, std::move(meta)) {}

    bool isEqual(const Node& other) const override { return other.isA<Function>() && Declaration::isEqual(other); }

    HILTI_NODE(Function)

private:
    DeclarationPtr _parent = nullptr;
};

} // namespace hilti::declaration
