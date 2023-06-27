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
    NodeFactory(ASTContext* context) : _context(context) {}

    ASTContext* context() const { return _context; }

private:
    ASTContext* _context;

public:
#include <hilti/ast/builder/node-factory.autogen.h>

    auto typeAuto(const Meta& meta = {}) { return QualifiedType::createAuto(context(), meta); }

    ExpressionPtr namedCtor(const std::string& name, const Expressions& args, const Meta& m = Meta()) {
        return expressionUnresolvedOperator(operator_::Kind::Call,
                                            {expressionMember(ID(name)), expressionCtor(ctorTuple(args))}, m);
    }

    ExpressionPtr expressionByName(const std::string& id, const Meta& m = Meta()) { return expressionName(ID(id, m)); }
};

} // namespace hilti::builder
