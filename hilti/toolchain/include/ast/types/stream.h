// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <utility>

#include <hilti/ast/type.h>
#include <hilti/ast/types/integer.h>

#include "ast/type.h"

namespace hilti::type {

namespace stream {

/** AST node for a stream iterator type. */
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

/** AST node for a stream view type. */
class View : public UnqualifiedType {
public:
    QualifiedTypePtr elementType() const final { return iteratorType()->dereferencedType(); }
    UnqualifiedTypePtr iteratorType() const final { return child<UnqualifiedType>(0); }

    Nodes typeParameters() const final { return children(); }

    static auto create(ASTContext* ctx, Meta meta = {}) {
        return NodeDerivedPtr<View>(new View({Iterator::create(ctx, meta)}, std::move(meta)));
    }

protected:
    View(Nodes children, Meta meta) : UnqualifiedType(std::move(children), std::move(meta)) {}

    bool _isAllocable() const final { return true; }
    bool _isIterator() const final { return true; }
    bool _isMutable() const final { return true; }

    bool isEqual(const Node& other) const final { return other.isA<Iterator>() && UnqualifiedType::isEqual(other); }

    HILTI_NODE(View)
};

} // namespace stream

/** AST node for a `stream` type. */
class Stream : public UnqualifiedType {
public:
    QualifiedTypePtr elementType() const final { return child<stream::Iterator>(0)->dereferencedType(); }
    UnqualifiedTypePtr iteratorType() const final { return child<stream::Iterator>(0)->iteratorType(); }

    static auto create(ASTContext* ctx, const Meta& meta = {}) {
        return NodeDerivedPtr<Stream>(new Stream({stream::Iterator::create(ctx, meta)}, meta));
    }

protected:
    Stream(Nodes children, Meta meta) : UnqualifiedType(std::move(children), std::move(meta)) {}
    Stream(Wildcard _, Meta meta) : UnqualifiedType(Wildcard(), std::move(meta)) {}

    bool _isAllocable() const final { return true; }
    bool _isMutable() const final { return true; }
    bool _isRuntimeNonTrivial() const final { return true; }
    bool _isSortable() const final { return true; }
    bool _isResolved(ResolvedState* rstate) const final { return true; }

    bool isEqual(const Node& other) const override { return other.isA<Stream>() && UnqualifiedType::isEqual(other); }

    HILTI_NODE(Stream)
};

} // namespace hilti::type
