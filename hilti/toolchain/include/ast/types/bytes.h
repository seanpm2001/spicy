// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/type.h>
#include <hilti/ast/types/integer.h>

namespace hilti::type {

namespace bytes {

/** AST node for a bytes iterator type. */
class Iterator : public UnqualifiedType {
public:
    QualifiedTypePtr dereferencedType() const final { return child<QualifiedType>(0); }
    Nodes typeParameters() const final { return children(); }

    static auto create(ASTContext* ctx, Meta meta = {}) {
        auto etype = QualifiedType::create(ctx, type::UnsignedInteger::create(ctx, 8, meta), true, meta);
        return NodeDerivedPtr<Iterator>(new Iterator({etype}, std::move(meta)));
    }

protected:
    Iterator(Nodes children, Meta meta) : UnqualifiedType(std::move(children), std::move(meta)) {}

    bool _isAllocable() const final { return true; }
    bool _isIterator() const final { return true; }
    bool _isMutable() const final { return true; }

    bool isEqual(const Node& other) const final { return other.isA<Iterator>() && UnqualifiedType::isEqual(other); }

    HILTI_NODE(Iterator)
};

} // namespace bytes

/** AST node for a `bytes` type. */
class Bytes : public UnqualifiedType {
public:
    QualifiedTypePtr elementType() const final { return child<bytes::Iterator>(0)->dereferencedType(); }
    UnqualifiedTypePtr iteratorType() const final { return child<UnqualifiedType>(0); }

    static auto create(ASTContext* ctx, const Meta& meta = {}) {
        return NodeDerivedPtr<Bytes>(new Bytes({bytes::Iterator::create(ctx, meta)}, meta));
    }

protected:
    Bytes(Nodes children, Meta meta) : UnqualifiedType(std::move(children), std::move(meta)) {}
    Bytes(Wildcard _, Meta meta) : UnqualifiedType(Wildcard(), std::move(meta)) {}

    bool _isAllocable() const final { return true; }
    bool _isMutable() const final { return true; }
    bool _isRuntimeNonTrivial() const final { return true; }
    bool _isSortable() const final { return true; }
    bool _isResolved(ResolvedState* rstate) const final { return true; }

    bool isEqual(const Node& other) const override { return other.isA<Bytes>() && UnqualifiedType::isEqual(other); }

    HILTI_NODE(Bytes)
};

} // namespace hilti::type
