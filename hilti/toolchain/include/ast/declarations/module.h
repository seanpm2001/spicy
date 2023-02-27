// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <string>
#include <utility>

#include <hilti/ast/declaration.h>
#include <hilti/ast/module.h>

namespace hilti::declaration {

/** AST node for a module declaration. */
class Module : public Declaration {
public:
    auto module_() const { return child<Module>(0); }

    std::string displayName() const final { return "module"; }

    static auto create(ASTContext* ctx, const ModulePtr& module_, Meta meta = {}) {
        return NodeDerivedPtr<Module>(new Module({module_}, module_->id(), std::move(meta)));
    }

protected:
    Module(Nodes children, ID id, Meta meta)
        : Declaration(std::move(children), std::move(id), Linkage::Public, std::move(meta)) {}

    bool isEqual(const Node& other) const override { return other.isA<Module>() && Declaration::isEqual(other); }

    HILTI_NODE(Module)
};

} // namespace hilti::declaration
