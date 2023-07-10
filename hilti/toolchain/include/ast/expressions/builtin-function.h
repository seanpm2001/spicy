// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <hilti/ast/expression.h>
#include <hilti/ast/type.h>
#include <hilti/ast/types/function.h>

namespace hilti::expression {

/** AST node representing a builtin function call. */
class BuiltInFunction : public Expression {
public:
    auto arguments() const { return children<Expression>(_num_parameters + 1, -1); }
    auto parameters() const { return children<declaration::Parameter>(1, _num_parameters); }
    const auto& cxxname() const { return _cxxname; }
    const auto& name() const { return _name; }

    QualifiedTypePtr type() const final { return child<QualifiedType>(0); }
    bool isLhs() const final { return false; }
    bool isTemporary() const final { return true; }

    node::Properties properties() const final {
        auto p = node::Properties{{"name", _name}, {"cxxname", _cxxname}};
        return Expression::properties() + p;
    }

    /** Construct a builtin function call node.
     *
     * @param name the name of the function on the HILTI side
     * @param cxxname the name of the wrapped C++ function
     * @param type the return type of the function
     * @param parameters parameters of the function
     * @param arguments arguments to the function call
     * @param m meta information for the function call
     */
    static auto create(ASTContext* ctx, const std::string& name, const std::string& cxxname,
                       const QualifiedTypePtr& type, const type::function::Parameters& parameters,
                       const Expressions& arguments, const Meta& meta = {}) {
        return NodeDerivedPtr<BuiltInFunction>(new BuiltInFunction(node::flatten(type, parameters, arguments), name,
                                                                   cxxname, static_cast<int>(parameters.size()), meta));
    }

protected:
    BuiltInFunction(Nodes children, std::string name, std::string cxxname, int num_parameters, Meta meta)
        : Expression(std::move(children), std::move(meta)),
          _name(std::move(name)),
          _cxxname(std::move(cxxname)),
          _num_parameters(num_parameters) {}

    bool isEqual(const Node& other) const override {
        auto n = other.tryAs<BuiltInFunction>();
        if ( ! n )
            return false;

        return Expression::isEqual(other) && _name == n->_name && _cxxname == n->_cxxname;
    }

    HILTI_NODE(BuiltInFunction)

private:
    std::string _name;
    std::string _cxxname;
    int _num_parameters = 0;
};

} // namespace hilti::expression
