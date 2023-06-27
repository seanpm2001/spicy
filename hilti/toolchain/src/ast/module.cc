// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#include <hilti/ast/declarations/property.h>
#include <hilti/ast/module.h>
#include <hilti/ast/statements/block.h>
#include <hilti/ast/visitor.h>

using namespace hilti;

Module::~Module() = default;

std::string Module::_render() const { return ""; }

void Module::clear() {
    assert(false && "Do we need this?? Need reimplementation");
#if 0
    auto v = visitor::PostOrder();

    // We fully walk the AST here in order to break any reference cycles it may
    // contain. Start at child 1 to leave ID in place.
    for ( size_t i = 0; i < children().size(); i++ ) {
        for ( auto j : v.walk(children()[i]) )
            j->reset();
    }

    clearChildren();
    addChild(statement::Block::create({}, meta()));
    clearDocumentation();
#endif
}

std::shared_ptr<declaration::Property> Module::moduleProperty(const ID& id) const {
    for ( const auto& d : declarations() ) {
        if ( ! d->isA<declaration::Property>() )
            return {};

        const auto& x = d->as<declaration::Property>();
        if ( x->id() == id )
            return {x};
    }

    return {};
}

node::Set<declaration::Property> Module::moduleProperties(const std::optional<ID>& id) const {
    node::Set<declaration::Property> props;

    for ( const auto& d : declarations() ) {
        if ( auto p = d->tryAs<declaration::Property>(); p && (! id || p->id() == id) )
            props.insert(p);
    }

    return props;
}
