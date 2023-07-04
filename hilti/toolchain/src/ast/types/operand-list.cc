// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#include <algorithm>
#include <exception>

#include <hilti/ast/types/operand-list.h>
#include <hilti/ast/expression.h>

using namespace hilti;
using namespace hilti::type;

std::string operand_list::Operand::print() const {
    auto t = (type ? type->print() : "<no-type>");

    std::string s;

    if ( id && *id )
        s = util::fmt("%s: %s", *id, t);
    else
        s = t;

    if ( default_ )
        s = util::fmt("%s=%s", s, *default_);

    if ( optional || default_ )
        s = util::fmt("[%s]", s);

    return s;
}
