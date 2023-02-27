// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <hilti/ast/ctor.h>
#include <hilti/ast/expression.h>
#include <hilti/ast/type.h>
#include <hilti/ast/types/bool.h>
#include <hilti/ast/types/map.h>

namespace hilti::ctor {

namespace map {

/** Base class for map field nodes. */
class Element final : public Node {
public:
    ~Element() final;

    auto key() const { return child<Expression>(0); }
    auto value() const { return child<Expression>(1); }

    static auto create(ASTContext* ctx, const ExpressionPtr& key, const ExpressionPtr& value, Meta meta = {}) {
        return NodeDerivedPtr<Element>(new Element({key, value}, std::move(meta)));
    }

protected:
    Element(Nodes children, Meta meta = {}) : Node(std::move(children), std::move(meta)) {}

    std::string _render() const final;

    bool isEqual(const Node& other) const override { return other.isA<Element>() && Node::isEqual(other); }

    HILTI_NODE(Element);
};

using ElementPtr = std::shared_ptr<Element>;
using Elements = std::vector<ElementPtr>;

} // namespace map

/** AST node for a `map` ctor. */
class Map : public Ctor {
public:
    auto value() const { return children<map::Element>(1, -1); }

    auto keyType() const {
        if ( auto t = child<type::Map>(0) )
            return t->keyType();
        else
            return child(0)->as<QualifiedType>();
    }

    auto valueType() const {
        if ( auto t = child<type::Map>(0) )
            return t->valueType();
        else
            return child(0)->as<QualifiedType>();
    }

    QualifiedTypePtr type() const final { return child<QualifiedType>(0); }

    static auto create(ASTContext* ctx, const QualifiedTypePtr& key, const QualifiedTypePtr& value,
                       map::Elements elements, const Meta& meta = {}) {
        auto mtype = QualifiedType::create(ctx, type::Map::create(ctx, key, value, meta), true, meta);
        return NodeDerivedPtr<Map>(new Map(node::flatten(mtype, std::move(elements)), meta));
    }

    static auto create(ASTContext* ctx, map::Elements elements, const Meta& meta = {}) {
        // bool is just an arbitrary place-holder type for empty values.
        auto mtype = elements.empty() ? QualifiedType::create(ctx, type::Bool::create(ctx, meta), true, meta) :
                                        QualifiedType::createAuto(ctx, meta);
        return NodeDerivedPtr<Map>(new Map(node::flatten(mtype, std::move(elements)), meta));
    }

protected:
    Map(Nodes children, Meta meta) : Ctor(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const override { return other.isA<Map>() && Ctor::isEqual(other); }

    HILTI_NODE(Map)
};

} // namespace hilti::ctor
