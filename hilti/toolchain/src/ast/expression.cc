// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#include <memory>

#include <hilti/ast/expression.h>
#include <hilti/ast/type.h>
#include <hilti/ast/visitor.h>

using namespace hilti;

Expression::~Expression() = default;

std::string Expression::_render() const {
    return util::fmt("%s %s", (type()->isConstant() ? " (const)" : " (non-const)"),
                     (expression::isResolved(*this) ? " (resolved)" : " (not resolved)"));
}

bool expression::isResolved(const Expression& e, type::ResolvedState* rstate) {
    // We always consider `self` expressions as fully resolved to break the
    // cycle with the type that they are pointing to.
    /*
       TODO
     * if ( auto id = e->tryAs<expression::ResolvedID>(); id && id->id() == ID("self") )
     *     return true;
     */

    return type::detail::isResolved(e.type(), rstate);
}
