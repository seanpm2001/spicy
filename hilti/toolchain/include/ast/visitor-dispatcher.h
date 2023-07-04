// Copyright (c) 2020-2021 by the Zeek Project. See LICENSE for details.

#pragma once

#include <hilti/ast/forward.h>

namespace hilti::visitor {

class Dispatcher {
public:
    virtual void operator()(hilti::ASTRoot*) {}
    virtual void operator()(hilti::Attribute*) {}
    virtual void operator()(hilti::AttributeSet*) {}
    virtual void operator()(hilti::Ctor*) {}
    virtual void operator()(hilti::Declaration*) {}
    virtual void operator()(hilti::Expression*) {}
    virtual void operator()(hilti::Function*) {}
    virtual void operator()(hilti::QualifiedType*) {}
    virtual void operator()(hilti::Statement*) {}
    virtual void operator()(hilti::UnqualifiedType*) {}
    virtual void operator()(hilti::ctor::Address*) {}
    virtual void operator()(hilti::ctor::Bool*) {}
    virtual void operator()(hilti::ctor::Bytes*) {}
    virtual void operator()(hilti::ctor::Coerced*) {}
    virtual void operator()(hilti::ctor::Default*) {}
    virtual void operator()(hilti::ctor::Enum*) {}
    virtual void operator()(hilti::ctor::Error*) {}
    virtual void operator()(hilti::ctor::Exception*) {}
    virtual void operator()(hilti::ctor::Interval*) {}
    virtual void operator()(hilti::ctor::Library*) {}
    virtual void operator()(hilti::ctor::List*) {}
    virtual void operator()(hilti::ctor::Map*) {}
    virtual void operator()(hilti::ctor::Network*) {}
    virtual void operator()(hilti::ctor::Null*) {}
    virtual void operator()(hilti::ctor::Optional*) {}
    virtual void operator()(hilti::ctor::Port*) {}
    virtual void operator()(hilti::ctor::Real*) {}
    virtual void operator()(hilti::ctor::RegExp*) {}
    virtual void operator()(hilti::ctor::Result*) {}
    virtual void operator()(hilti::ctor::Set*) {}
    virtual void operator()(hilti::ctor::SignedInteger*) {}
    virtual void operator()(hilti::ctor::Stream*) {}
    virtual void operator()(hilti::ctor::String*) {}
    virtual void operator()(hilti::ctor::StrongReference*) {}
    virtual void operator()(hilti::ctor::Struct*) {}
    virtual void operator()(hilti::ctor::Time*) {}
    virtual void operator()(hilti::ctor::Tuple*) {}
    virtual void operator()(hilti::ctor::Union*) {}
    virtual void operator()(hilti::ctor::UnsignedInteger*) {}
    virtual void operator()(hilti::ctor::ValueReference*) {}
    virtual void operator()(hilti::ctor::Vector*) {}
    virtual void operator()(hilti::ctor::WeakReference*) {}
    virtual void operator()(hilti::ctor::map::Element*) {}
    virtual void operator()(hilti::ctor::struct_::Field*) {}
    virtual void operator()(hilti::declaration::Constant*) {}
    virtual void operator()(hilti::declaration::Expression*) {}
    virtual void operator()(hilti::declaration::Field*) {}
    virtual void operator()(hilti::declaration::Function*) {}
    virtual void operator()(hilti::declaration::GlobalVariable*) {}
    virtual void operator()(hilti::declaration::ImportedModule*) {}
    virtual void operator()(hilti::declaration::LocalVariable*) {}
    virtual void operator()(hilti::declaration::Module*) {}
    virtual void operator()(hilti::declaration::Property*) {}
    virtual void operator()(hilti::declaration::Type*) {}
    virtual void operator()(hilti::expression::Assign*) {}
    virtual void operator()(hilti::expression::BuiltInFunction*) {}
    virtual void operator()(hilti::expression::Coerced*) {}
    virtual void operator()(hilti::expression::Ctor*) {}
    virtual void operator()(hilti::expression::Deferred*) {}
    virtual void operator()(hilti::expression::Grouping*) {}
    virtual void operator()(hilti::expression::Keyword*) {}
    virtual void operator()(hilti::expression::ListComprehension*) {}
    virtual void operator()(hilti::expression::LogicalAnd*) {}
    virtual void operator()(hilti::expression::LogicalNot*) {}
    virtual void operator()(hilti::expression::LogicalOr*) {}
    virtual void operator()(hilti::expression::Member*) {}
    virtual void operator()(hilti::expression::Move*) {}
    virtual void operator()(hilti::expression::Name*) {}
    virtual void operator()(hilti::expression::PendingCoerced*) {}
    virtual void operator()(hilti::expression::ResolvedOperator*) {}
    virtual void operator()(hilti::expression::Ternary*) {}
    virtual void operator()(hilti::expression::TypeInfo*) {}
    virtual void operator()(hilti::expression::TypeWrapped*) {}
    virtual void operator()(hilti::expression::Type_*) {}
    virtual void operator()(hilti::expression::UnresolvedOperator*) {}
    virtual void operator()(hilti::expression::Void*) {}
    virtual void operator()(hilti::operator_::function::Call*) {}
    virtual void operator()(hilti::statement::Assert*) {}
    virtual void operator()(hilti::statement::Block*) {}
    virtual void operator()(hilti::statement::Break*) {}
    virtual void operator()(hilti::statement::Comment*) {}
    virtual void operator()(hilti::statement::Continue*) {}
    virtual void operator()(hilti::statement::Declaration*) {}
    virtual void operator()(hilti::statement::Expression*) {}
    virtual void operator()(hilti::statement::For*) {}
    virtual void operator()(hilti::statement::If*) {}
    virtual void operator()(hilti::statement::Return*) {}
    virtual void operator()(hilti::statement::SetLocation*) {}
    virtual void operator()(hilti::statement::Switch*) {}
    virtual void operator()(hilti::statement::Throw*) {}
    virtual void operator()(hilti::statement::Try*) {}
    virtual void operator()(hilti::statement::While*) {}
    virtual void operator()(hilti::statement::Yield*) {}
    virtual void operator()(hilti::statement::switch_::Case*) {}
    virtual void operator()(hilti::statement::try_::Catch*) {}
    virtual void operator()(hilti::type::Address*) {}
    virtual void operator()(hilti::type::Any*) {}
    virtual void operator()(hilti::type::Auto*) {}
    virtual void operator()(hilti::type::Bool*) {}
    virtual void operator()(hilti::type::Bytes*) {}
    virtual void operator()(hilti::type::DocOnly*) {}
    virtual void operator()(hilti::type::Enum*) {}
    virtual void operator()(hilti::type::Error*) {}
    virtual void operator()(hilti::type::Exception*) {}
    virtual void operator()(hilti::type::Function*) {}
    virtual void operator()(hilti::type::Interval*) {}
    virtual void operator()(hilti::type::Library*) {}
    virtual void operator()(hilti::type::List*) {}
    virtual void operator()(hilti::type::Map*) {}
    virtual void operator()(hilti::type::Member*) {}
    virtual void operator()(hilti::type::Name*) {}
    virtual void operator()(hilti::type::Network*) {}
    virtual void operator()(hilti::type::Null*) {}
    virtual void operator()(hilti::type::OperandList*) {}
    virtual void operator()(hilti::type::Optional*) {}
    virtual void operator()(hilti::type::Port*) {}
    virtual void operator()(hilti::type::Real*) {}
    virtual void operator()(hilti::type::RegExp*) {}
    virtual void operator()(hilti::type::Result*) {}
    virtual void operator()(hilti::type::Set*) {}
    virtual void operator()(hilti::type::SignedInteger*) {}
    virtual void operator()(hilti::type::Stream*) {}
    virtual void operator()(hilti::type::String*) {}
    virtual void operator()(hilti::type::StrongReference*) {}
    virtual void operator()(hilti::type::Struct*) {}
    virtual void operator()(hilti::type::Time*) {}
    virtual void operator()(hilti::type::Tuple*) {}
    virtual void operator()(hilti::type::Type_*) {}
    virtual void operator()(hilti::type::Union*) {}
    virtual void operator()(hilti::type::Unknown*) {}
    virtual void operator()(hilti::type::UnsignedInteger*) {}
    virtual void operator()(hilti::type::ValueReference*) {}
    virtual void operator()(hilti::type::Vector*) {}
    virtual void operator()(hilti::type::Void*) {}
    virtual void operator()(hilti::type::WeakReference*) {}
    virtual void operator()(hilti::type::bytes::Iterator*) {}
    virtual void operator()(hilti::type::enum_::Label*) {}
    virtual void operator()(hilti::type::function::Parameter*) {}
    virtual void operator()(hilti::type::list::Iterator*) {}
    virtual void operator()(hilti::type::map::Iterator*) {}
    virtual void operator()(hilti::type::set::Iterator*) {}
    virtual void operator()(hilti::type::stream::Iterator*) {}
    virtual void operator()(hilti::type::stream::View*) {}
    virtual void operator()(hilti::type::tuple::Element*) {}
    virtual void operator()(hilti::type::vector::Iterator*) {}
};

} // namespace hilti::visitor
