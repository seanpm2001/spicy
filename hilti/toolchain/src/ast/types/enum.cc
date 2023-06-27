// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#include <algorithm>
#include <exception>

#include <hilti/ast/ctors/enum.h>
#include <hilti/ast/declarations/constant.h>
#include <hilti/ast/expressions/ctor.h>
#include <hilti/ast/types/enum.h>

using namespace hilti;

type::enum_::Label::~Label() {}

std::string type::enum_::Label::_render() const { return util::fmt("[id: %s value: %" PRIu64 "]", _id, _value); }

Declarations type::Enum::_normalizeLabels(ASTContext* ctx, enum_::Labels labels) {
    auto max = std::max_element(labels.begin(), labels.end(),
                                [](const auto& l1, const auto& l2) { return l1->value() < l2->value(); });
    auto next_value = (max != labels.end() ? (*max)->value() + 1 : 0);

    Declarations nlabels;

    for ( auto&& l : labels ) {
        if ( util::tolower(l->id()) == "undef" )
            throw std::out_of_range("reserved enum label 'Undef' cannot be redefined");

        std::shared_ptr<type::enum_::Label> nlabel;

        if ( l->value() < 0 )
            nlabel = type::enum_::Label::create(ctx, l->id(), next_value++, l->meta())->as<type::enum_::Label>();
        else
            nlabel = std::move(l);

        auto d = declaration::Constant::create(ctx, nlabel->id(),
                                               expression::Ctor::create(ctx, ctor::Enum::create(ctx, nlabel)));
        nlabels.push_back(std::move(d));
    }

    auto undef_label = type::enum_::Label::create(ctx, ID("Undef"), -1)->as<type::enum_::Label>();
    auto undef = declaration::Constant::create(ctx, undef_label->id(),
                                               expression::Ctor::create(ctx, ctor::Enum::create(ctx, undef_label)));
    nlabels.emplace_back(std::move(std::move(undef)));

    return nlabels;
}

type::enum_::Labels type::Enum::labels() const {
    enum_::Labels labels;

    for ( const auto& c : children() ) {
        auto label = c->as<declaration::Constant>()->value()->as<expression::Ctor>()->ctor()->as<ctor::Enum>()->value();
        labels.emplace_back(label);
    }

    return labels;
}

type::enum_::Labels type::Enum::uniqueLabels() const {
    auto pred_gt = [](const auto& e1, const auto& e2) { return e1->value() > e2->value(); };
    auto pred_eq = [](const auto& e1, const auto& e2) { return e1->value() == e2->value(); };

    auto in = labels();
    enum_::Labels out;
    std::copy(in.begin(), in.end(), std::back_inserter(out));
    std::sort(out.begin(), out.end(), pred_gt);
    out.erase(std::unique(out.begin(), out.end(), pred_eq), out.end());
    return out;
}

void type::Enum::initLabelTypes(ASTContext* ctx, const NodePtr& n) {
    auto etype = n->as<type::Enum>();

    Nodes nlabels;
    for ( const auto& l : etype->labels() ) {
        auto nlabel = enum_::Label::create(ctx, l->id(), l->value(), etype, l->meta())->as<enum_::Label>();
        auto d = declaration::Constant::create(ctx, nlabel->id(),
                                               expression::Ctor::create(ctx, ctor::Enum::create(ctx, nlabel)));
        nlabels.emplace_back(std::move(d));
    }

    n->replaceChildren(nlabels);
    etype->_initialized = true;
}
