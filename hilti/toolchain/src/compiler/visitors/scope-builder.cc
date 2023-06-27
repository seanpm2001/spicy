// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#include <utility>

#include <hilti/ast/declaration.h>
#include <hilti/ast/declarations/expression.h>
#include <hilti/ast/declarations/global-variable.h>
#include <hilti/ast/declarations/imported-module.h>
#include <hilti/ast/declarations/local-variable.h>
#include <hilti/ast/declarations/module.h>
#include <hilti/ast/expressions/keyword.h>
#include <hilti/ast/expressions/list-comprehension.h>
#include <hilti/ast/statements/declaration.h>
#include <hilti/ast/types/function.h>
#include <hilti/ast/types/reference.h>
#include <hilti/ast/types/struct.h>
#include <hilti/ast/types/unresolved-id.h>
#include <hilti/compiler/detail/visitors.h>
#include <hilti/compiler/unit.h>

#include "ast/builder/builder.h"
#include "ast/declarations/type.h"
#include "base/timing.h"

using namespace hilti;

namespace {

struct Visitor : visitor::PostOrder {
    explicit Visitor(Builder* builder, const ASTRootPtr& root) : root(root), builder(builder) {}

    const ASTRootPtr& root;
    Builder* builder;

    void operator()(declaration::Module* m) final {
        // Insert module name into global scope.
        root->getOrCreateScope()->insert(m->as<declaration::Module>());
    }

    void operator()(declaration::GlobalVariable* d) final {
        if ( d->parent()->isA<declaration::Module>() )
            d->parent()->getOrCreateScope()->insert(d->as<declaration::GlobalVariable>());
    }

    void operator()(declaration::Type* d) final {
        if ( d->parent()->isA<declaration::Module>() )
            d->parent()->getOrCreateScope()->insert(d->as<declaration::Type>());
    }

    void operator()(declaration::Constant* d) final {
        if ( d->parent()->isA<declaration::Module>() )
            d->parent()->getOrCreateScope()->insert(d->as<declaration::Constant>());
    }

    void operator()(declaration::Expression* d) final {
        if ( d->parent()->isA<declaration::Module>() )
            d->parent()->getOrCreateScope()->insert(d->as<declaration::Expression>());
    }

#if 0
    void operator()(declaration::Field* f) final {
        if ( auto func = f.inlineFunction() ) {
            for ( auto&& x : func->ftype().parameterRefs() )
                p.node.scope()->insert(std::move(x));
        }

        if ( f.isStatic() )
            // Insert static member into struct's namespace. We create new
            // declarations here (rather than point to instances already
            // existing inside the AST) as that's (a) easier and (b) ok
            // because everything is checked to be fully resolved already.
            //
            p.parent(2).scope()->insert(NodeRef(p.node));
    }

    void operator()(declaration::Function* f) final {
        if ( d.parent()->isA<Module>() )
            d.parent()->scope()->insert(NodeRef(p.node));

        for ( auto&& x : f.function().ftype().parameterRefs() )
            p.node.scope()->insert(std::move(x));

        if ( f.linkage() == declaration::Linkage::Struct ) {
            if ( ! f.id().namespace_() ) {
                p.node.addError("method lacks a type namespace");
                return;
            }
        }

        if ( f.linkage() == declaration::Linkage::Struct && f.parentStructType() && f.parentStructType()->selfRef() ) {
            const auto& t = *f.parentStructType();
            auto ns = f.id().namespace_();

            auto fields = t.fields(f.id().local());
            if ( fields.empty() ) {
                p.node.addError(util::fmt("type %s does not have a method '%s'", ns, f.id().local()));
                return;
            }

            bool found = false;
            for ( const auto& sf : fields ) {
                auto sft = sf.type().tryAs<type::Function>();

                if ( ! sft ) {
                    p.node.addError(util::fmt("%s is not a method", ID(ns, f.id().local())));
                    return;
                }

                if ( areEquivalent(*sft, f.function().ftype()) )
                    found = true;
            }

            if ( ! found ) {
                p.node.addError(
                    util::fmt("type %s does not have a method '%s' matching the signature", ns, f.id().local()));
                return;
            }

            p.node.scope()->insert(t.selfRef());

            for ( auto&& x : t.parameterRefs() )
                p.node.scope()->insert(NodeRef(x));
        }
    }

    void operator()(declaration::ImportedModule* m) final {
        if ( const auto& cached = context->lookupUnit(m.id(), m.scope(), unit->extension()) ) {
            auto other = cached->unit->moduleRef();
            p.node.setScope(other->scope());
            auto n = unit->module().as<Module>().preserve(p.node);
            const_cast<Node&>(*n).setScope(other->scope());
            d.parent()->scope()->insert(std::move(n));
        }
    }

    void operator()(expression::ListComprehension* e) final { p.node.scope()->insert(e.localRef()); }

    void operator()(statement::Declaration* d) final { d.parent()->scope()->insert(d.declarationRef()); }

    void operator()(statement::For* s) final { p.node.scope()->insert(s.localRef()); }

    void operator()(statement::If* s) final {
        if ( s.initRef() )
            p.node.scope()->insert(s.initRef());
    }

    void operator()(statement::Switch* s) final { p.node.scope()->insert(s.conditionRef()); }

    void operator()(statement::try_::Catch* s) final {
        if ( auto x = s.parameterRef() )
            p.node.scope()->insert(std::move(x));
    }

    void operator()(statement::While* s) final {
        if ( auto x = s.initRef() )
            p.node.scope()->insert(std::move(x));
    }

    void operator()(type::Enum* m) final {
        if ( ! d.parent()->isA<declaration::Type>() )
            return;

        if ( ! p.node.as<Type>().typeID() )
            // We need to associate the type ID with the declaration we're
            // creating, so wait for that to have been set by the resolver.
            return;

        for ( auto&& d : p.node.as<type::Enum>().labelDeclarationRefs() )
            d.parent()->scope()->insert(std::move(d));
    }

    void operator()(type::Struct* t) final {
        if ( ! p.node.as<Type>().typeID() )
            // We need to associate the type ID with the declaration we're
            // creating, so wait for that to have been set by the resolver.
            return;

        if ( t.selfRef() )
            p.node.scope()->insert(t.selfRef());

        for ( auto&& x : t.parameterRefs() )
            d.parent()->scope()->insert(std::move(x));
    }
#endif
};

} // anonymous namespace

void hilti::detail::ast::buildScopes(Builder* builder, const ASTRootPtr& root) {
    util::timing::Collector _("hilti/compiler/ast/scope-builder");
    ::hilti::visitor::visit(Visitor(builder, root), root);
}
