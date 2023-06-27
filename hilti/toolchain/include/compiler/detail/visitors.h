// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <hilti/ast/ctor.h>
#include <hilti/ast/expression.h>
#include <hilti/ast/expressions/resolved-operator.h>
#include <hilti/ast/expressions/unresolved-operator.h>
#include <hilti/ast/function.h>
#include <hilti/ast/node.h>
#include <hilti/ast/operator.h>
#include <hilti/base/logger.h>
#include <hilti/compiler/context.h>

namespace hilti {

class Unit;

namespace printer {
class Stream;
} // namespace printer

namespace detail {

/**
 * Prints an AST as HILTI source code. This consults any installed plugin
 * `print_ast` hooks.
 */
void printAST(const NodePtr& root, std::ostream& out, bool compact = false);

/**
 * Prints an AST as HILTI source code. This consults any installed plugin
 * `print_ast` hooks.
 */
void printAST(const NodePtr& root, printer::Stream& stream); // NOLINT

/** Returns a string with the prototype for an operator for display. */
std::string renderOperatorPrototype(const NodeDerivedPtr<expression::UnresolvedOperator>& o);

/** Returns a string with the prototype for an operator for display. */
std::string renderOperatorPrototype(const NodeDerivedPtr<expression::ResolvedOperator>& o);

/** Returns a string with an instantiated  operator for display. */
std::string renderOperatorInstance(const NodeDerivedPtr<expression::UnresolvedOperator>& o);

/** Returns a string with an instantiated  operator for display. */
std::string renderOperatorInstance(const NodeDerivedPtr<expression::ResolvedOperator>& o);

/** Prints a debug dump of a node, including its childrens. */
void renderNode(const NodePtr& n, std::ostream& out, bool include_scopes = false);
void renderNode(const NodePtr& n, logging::DebugStream stream, bool include_scopes = false);

/**
 * Folds an expression into a constant value if that's possible. Note that the
 * current implementation is very, very basic, and covers just a few cases. If
 * the function returns an error, that does not necessarily mean that the
 * expression is not represeneting a constant value, but only that we aren't
 * able to compute it.
 */
Result<CtorPtr> foldConstant(const ExpressionPtr& expr);

namespace ast {
/** Implements the corresponding functionality for the default HILTI compiler plugin. */
void buildScopes(Builder* builder, const ASTRootPtr& root);
/** Implements the corresponding functionality for the default HILTI compiler plugin. */
bool normalize(Builder* builder, const ASTRootPtr& root);
/** Implements the corresponding functionality for the default HILTI compiler plugin. */
bool coerce(Builder* builder, const ASTRootPtr& root);
/** Implements the corresponding functionality for the default HILTI compiler plugin. */
bool resolve(Builder* builder, const ASTRootPtr& root);
/** Implements the corresponding functionality for the default HILTI compiler plugin. */
void validate_pre(ASTContext* ctx, const ASTRootPtr& root);
/** Implements the corresponding functionality for the default HILTI compiler plugin. */
void validate_post(ASTContext* ctx, const ASTRootPtr& root);
} // namespace ast
} // namespace detail
} // namespace hilti
