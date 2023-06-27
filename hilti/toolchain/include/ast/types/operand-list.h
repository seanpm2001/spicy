// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>
#include <vector>

#include <hilti/ast/type.h>

namespace hilti::type {

namespace operand_list {

struct Operand {
    std::optional<ID> id;
    QualifiedTypePtr type;
    bool optional = false;
    ExpressionPtr default_;

    bool operator==(const Operand& other) const {
        return id == other.id && type == other.type && optional == other.optional &&
               ((default_ && other.default_ && default_ == other.default_) || (! default_ && ! other.default_));
    }
};

using Operands = std::vector<Operand>;
} // namespace operand_list

/**
 * AST node for a type representing a list of function/method operands. This
 * is an internal type used for overload resolution, it's nothing actually
 * instantiated by a HILTI program. That's also why we don't use any child
 * nodes, but store the operands directly.
 */
class OperandList : public UnqualifiedType {
public:
    const auto& operands() const { return _operands; }
    const auto& op0() const { return _operands[0]; }
    const auto& op1() const { return _operands[1]; }
    const auto& op2() const { return _operands[2]; }

    static auto create(ASTContext* ctx, operand_list::Operands operands, Meta meta = {}) {
        return NodeDerivedPtr<OperandList>(new OperandList(std::move(operands), std::move(meta)));
    }

    static auto create(ASTContext* ctx, Wildcard _, Meta m = Meta()) {
        return NodeDerivedPtr<OperandList>(new OperandList(Wildcard(), std::move(m)));
    }

    template<typename Container>
    static QualifiedTypePtr fromParameters(ASTContext* ctx, const Container& params) {
        operand_list::Operands ops;

        for ( const auto& p : params )
            ops.push_back(operand_list::Operand{.id = p->id(),
                                                .type = p->type(),
                                                .optional = (p->default_() != nullptr),
                                                .default_ = p->default_()});

        return QualifiedType::create(ctx, type::OperandList::create(ctx, std::move(ops)), true);
    }

protected:
    OperandList(operand_list::Operands operands, Meta meta)
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
    operand_list::Operands _operands;
};

} // namespace hilti::type
