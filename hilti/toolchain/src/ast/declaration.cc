// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#include <hilti/ast/declaration.h>
#include <hilti/ast/visitor.h>

using namespace hilti;

Declaration::~Declaration() = default;

std::string Declaration::_render() const {
    auto s = util::fmt("[canon-id: %s]", canonicalID() ? canonicalID().str() : "not set");

    if ( auto doc = documentation() )
        s += doc->render();

    return s;
}
