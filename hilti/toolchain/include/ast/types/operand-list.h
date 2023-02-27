// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/operator.h>
#include <hilti/ast/type.h>

namespace hilti::type {

/**
 * AST node for a type representing a list of function/method operands. This
 * is an internal type used for overload resolution, it's nothing actually
 * instantiated by a HILTI program. That's also why we don't use any child
 * nodes, but store the operands directly.
 */
class OperandList : public UnqualifiedType {
public:
    const auto& operands() const { return _operands; }

    // TODO
#if 0
    template<typename Container>
    static OperandList fromParameters(ASTContext* ctx, const Container& params) {
        operator_::Operands ops;

        for ( const auto& p : params ) {
            operator_::Operand op = {p.id(), (p.isConstant() ? type::constant(p.type()) : p.type()),
                                     p.default_().has_value(), p.default_()};

            ops.push_back(std::move(op));
        }

        return type::OperandList::create(ctx, std::move(ops));
    }
#endif

    static auto create(ASTContext* ctx, operator_::Operands operands, Meta meta = {}) {
        return NodeDerivedPtr<OperandList>(new OperandList(std::move(operands), std::move(meta)));
    }

    static auto create(ASTContext* ctx, Wildcard _, Meta m = Meta()) {
        return NodeDerivedPtr<OperandList>(new OperandList(Wildcard(), std::move(m)));
    }

protected:
    OperandList(operator_::Operands operands, Meta meta)
        : UnqualifiedType(std::move(meta)), _operands(std::move(operands)) {}
    OperandList(Wildcard _, Meta meta) : UnqualifiedType(Wildcard(), std::move(meta)) {}

    bool isEqual(const Node& other) const override {
        auto n = other.tryAs<OperandList>();
        if ( ! n )
            return false;

        return UnqualifiedType::isEqual(other) && _operands == n->_operands;
    }

    HILTI_NODE(OperandList)

private:
    operator_::Operands _operands;
};

} // namespace hilti::type
