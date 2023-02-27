// Copyright (c) 2021-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <string>
#include <utility>

#include <hilti/ast/all.h>
#include <hilti/ast/ast-context.h>

namespace hilti::builder {

class NodeFactory {
public:
    NodeFactory(std::shared_ptr<ASTContext> context) : _context(std::move(context)) {}

    ASTContext* context() const { return _context.get(); }

private:
    const std::shared_ptr<ASTContext> _context;

public:
#include <hilti/ast/builder/node-factory.autogen.h>

    auto typeAuto(const Meta& meta = {}) { return QualifiedType::createAuto(context(), meta); }

    ExpressionPtr namedCtor(const std::string& name, const Expressions& args, const Meta& m = Meta()) {
        return expressionUnresolvedOperator(operator_::Kind::Call,
                                            {expressionMember(ID(name)), expressionCtor(ctorTuple(args))}, m);
    }
};

} // namespace hilti::builder
