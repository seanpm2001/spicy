// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#include <functional>

#include <hilti/ast/declarations/all.h>
#include <hilti/ast/expressions/name.h>
#include <hilti/ast/types/type.h>
#include <hilti/base/logger.h>

using namespace hilti;

QualifiedTypePtr expression::Name::type() const {
    if ( ! _declaration )
        return child<QualifiedType>(0);

    struct Visitor : hilti::visitor::PreOrder {
        QualifiedTypePtr result = nullptr;

        void operator()(declaration::Constant* c) final { result = c->type(); }
        void operator()(declaration::Expression* e) final { result = e->expression()->type(); }
        void operator()(declaration::Field* f) final { result = f->type(); }
        void operator()(declaration::Function* f) final { result = f->function()->type(); }
        void operator()(declaration::GlobalVariable* v) final { result = v->type(); }
        void operator()(declaration::LocalVariable* v) final { result = v->type(); }
        void operator()(declaration::Parameter* p) final { result = p->type(); }
        void operator()(declaration::Type* t) final { result = t->type(); }
    };

    if ( auto type = visitor::dispatch(Visitor(), _declaration, [](const auto& x) { return x.result; }) )
        return type;

    logger().internalError(util::fmt("unsupported declaration type %s", _declaration->typename_()), this);
}
