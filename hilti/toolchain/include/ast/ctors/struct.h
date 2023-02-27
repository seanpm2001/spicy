// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <hilti/ast/ctor.h>
#include <hilti/ast/types/struct.h>

namespace hilti::ctor {

namespace struct_ {

/** Base class for struct field nodes. */
class Field final : public Node {
public:
    ~Field() final;

    const auto& id() const { return _id; }
    auto expression() const { return child<Expression>(0); }

    node::Properties properties() const override {
        auto p = node::Properties{{"id", _id}};
        return Node::properties() + p;
    }

    static auto create(ASTContext* ctx, ID id, const ExpressionPtr& expr, Meta meta = {}) {
        return NodeDerivedPtr<Field>(new Field({expr}, std::move(id), std::move(meta)));
    }

protected:
    Field(Nodes children, ID id, Meta meta = {}) : Node(std::move(children), std::move(meta)), _id(std::move(id)) {}

    std::string _render() const final;

    bool isEqual(const Node& other) const override { return other.isA<Field>() && Node::isEqual(other); }

    HILTI_NODE(Field);

private:
    ID _id;
};

using FieldPtr = std::shared_ptr<Field>;
using Fields = std::vector<FieldPtr>;

} // namespace struct_

/** AST node for a `struct` ctor. */
class Struct : public Ctor {
public:
    QualifiedTypePtr type() const final { return child<QualifiedType>(0); }

    auto stype() const { return type()->type()->as<type::Struct>(); }

    /** Returns all fields that the constructors initialized. */
    auto fields() const { return children<struct_::Field>(1, -1); }

    /** Returns a field initialized by the constructor by its ID. */
    struct_::FieldPtr field(const ID& id) const {
        for ( const auto& f : fields() ) {
            if ( f->id() == id )
                return f;
        }

        return nullptr;
    }

    static auto create(ASTContext* ctx, struct_::Fields fields, QualifiedTypePtr t, const Meta& meta = {}) {
        return NodeDerivedPtr<Struct>(new Struct(node::flatten(std::move(t), std::move(fields)), meta));
    }

    static auto create(ASTContext* ctx, struct_::Fields fields, const Meta& meta = {}) {
        return NodeDerivedPtr<Struct>(
            new Struct(node::flatten(QualifiedType::createAuto(ctx, meta), std::move(fields)), meta));
    }

protected:
    Struct(Nodes children, Meta meta) : Ctor(std::move(children), std::move(meta)) {}

    bool isEqual(const Node& other) const override { return other.isA<Struct>() && Ctor::isEqual(other); }

    HILTI_NODE(Struct)
};

} // namespace hilti::ctor
