auto attribute(std::string tag, Meta m = Meta()) {
    return hilti::Attribute::create(context(), tag, m);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/attribute.h:83:5
auto attribute(std::string tag, const NodePtr& v, Meta m = Meta()) {
    return hilti::Attribute::create(context(), tag, v, m);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/attribute.h:73:5
auto attributeSet(Attributes attrs, Meta m = Meta()) {
    return hilti::AttributeSet::create(context(), attrs, m);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/attribute.h:154:5
auto ctorAddress(hilti::rt::Address v, const Meta& meta = {}) {
    return hilti::ctor::Address::create(context(), v, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/address.h:29:5
auto ctorBool(bool v, const Meta& meta = {}) {
    return hilti::ctor::Bool::create(context(), v, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/bool.h:25:5
auto ctorBytes(std::string value, const Meta& meta = {}) {
    return hilti::ctor::Bytes::create(context(), value, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/bytes.h:26:5
auto ctorCoerced(const CtorPtr& orig, const CtorPtr& new_, const Meta& meta = {}) {
    return hilti::ctor::Coerced::create(context(), orig, new_, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/coerced.h:22:5
auto ctorDefault(const UnqualifiedTypePtr& type, Expressions type_args, const Meta& meta = {}) {
    return hilti::ctor::Default::create(context(), type, type_args, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/default.h:29:5
auto ctorDefault(const UnqualifiedTypePtr& type, const Meta& meta = {}) {
    return hilti::ctor::Default::create(context(), type, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/default.h:21:5
auto ctorEnum(const type::enum_::LabelPtr& label, const Meta& meta = {}) {
    return hilti::ctor::Enum::create(context(), label, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/enum.h:22:5
auto ctorError(std::string v, const Meta& meta = {}) {
    return hilti::ctor::Error::create(context(), v, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/error.h:26:5
auto ctorException(const UnqualifiedTypePtr& type, Expressions type_args, const Meta& meta = {}) {
    return hilti::ctor::Exception::create(context(), type, type_args, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/exception.h:29:5
auto ctorException(const UnqualifiedTypePtr& type, const Meta& meta = {}) {
    return hilti::ctor::Exception::create(context(), type, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/exception.h:21:5
auto ctorInterval(hilti::rt::Interval v, const Meta& meta = {}) {
    return hilti::ctor::Interval::create(context(), v, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/interval.h:27:5
auto ctorLibrary(const CtorPtr& ctor, const QualifiedTypePtr& type, const Meta& meta = {}) {
    return hilti::ctor::Library::create(context(), ctor, type, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/library.h:27:5
auto ctorList(Expressions exprs, const Meta& meta = {}) {
    return hilti::ctor::List::create(context(), exprs, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/list.h:28:5
auto ctorList(const QualifiedTypePtr& etype, Expressions exprs, const Meta& meta = {}) {
    return hilti::ctor::List::create(context(), etype, exprs, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/list.h:23:5
auto ctorMap(const QualifiedTypePtr& key, const QualifiedTypePtr& value, ctor::map::Elements elements,
             const Meta& meta = {}) {
    return hilti::ctor::Map::create(context(), key, value, elements, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/map.h:68:5
auto ctorMap(ctor::map::Elements elements, const Meta& meta = {}) {
    return hilti::ctor::Map::create(context(), elements, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/map.h:74:5
auto ctorMapElement(const ExpressionPtr& key, const ExpressionPtr& value, Meta meta = {}) {
    return hilti::ctor::map::Element::create(context(), key, value, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/map.h:28:5
auto ctorNetwork(hilti::rt::Network v, const Meta& meta = {}) {
    return hilti::ctor::Network::create(context(), v, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/network.h:27:5
auto ctorNull(const Meta& meta = {}) {
    return hilti::ctor::Null::create(context(), meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/null.h:18:5
auto ctorOptional(const ExpressionPtr& expr, const Meta& meta = {}) {
    return hilti::ctor::Optional::create(context(), expr, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/optional.h:29:5
auto ctorOptional(const QualifiedTypePtr& type, const Meta& meta = {}) {
    return hilti::ctor::Optional::create(context(), type, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/optional.h:39:5
auto ctorPort(hilti::rt::Port v, const Meta& meta = {}) {
    return hilti::ctor::Port::create(context(), v, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/port.h:27:5
auto ctorReal(double v, const Meta& meta = {}) {
    return hilti::ctor::Real::create(context(), v, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/real.h:25:5
auto ctorRegExp(std::vector<std::string> v, const Meta& meta = {}) {
    return hilti::ctor::RegExp::create(context(), v, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/regexp.h:27:5
auto ctorResult(const ExpressionPtr& expr, const Meta& meta = {}) {
    return hilti::ctor::Result::create(context(), expr, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/result.h:46:5
auto ctorSet(Expressions exprs, const Meta& meta = {}) {
    return hilti::ctor::Set::create(context(), exprs, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/set.h:28:5
auto ctorSet(const QualifiedTypePtr& etype, Expressions exprs, const Meta& meta = {}) {
    return hilti::ctor::Set::create(context(), etype, exprs, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/set.h:23:5
auto ctorSignedInteger(int64_t value, int width, const Meta& meta = {}) {
    return hilti::ctor::SignedInteger::create(context(), value, width, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/integer.h:50:5
auto ctorStream(std::string value, const Meta& meta = {}) {
    return hilti::ctor::Stream::create(context(), value, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/stream.h:26:5
auto ctorString(std::string value, const Meta& meta = {}) {
    return hilti::ctor::String::create(context(), value, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/string.h:26:5
auto ctorStrongReference(const QualifiedTypePtr& t, const Meta& meta = {}) {
    return hilti::ctor::StrongReference::create(context(), t, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/reference.h:25:5
auto ctorStruct(ctor::struct_::Fields fields, QualifiedTypePtr t, const Meta& meta = {}) {
    return hilti::ctor::Struct::create(context(), fields, t, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/struct.h:72:5
auto ctorStruct(ctor::struct_::Fields fields, const Meta& meta = {}) {
    return hilti::ctor::Struct::create(context(), fields, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/struct.h:76:5
auto ctorStructField(ID id, const ExpressionPtr& expr, Meta meta = {}) {
    return hilti::ctor::struct_::Field::create(context(), id, expr, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/struct.h:30:5
auto ctorTime(hilti::rt::Time v, const Meta& meta = {}) {
    return hilti::ctor::Time::create(context(), v, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/time.h:27:5
auto ctorTuple(const Expressions& exprs, const Meta& meta = {}) {
    return hilti::ctor::Tuple::create(context(), exprs, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/tuple.h:30:5
auto ctorUnion(const QualifiedTypePtr& type, const ExpressionPtr& value, const Meta& meta = {}) {
    return hilti::ctor::Union::create(context(), type, value, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/union.h:21:5
auto ctorUnsignedInteger(uint64_t value, int width, const Meta& meta = {}) {
    return hilti::ctor::UnsignedInteger::create(context(), value, width, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/integer.h:70:5
auto ctorUnsignedInteger(uint64_t value, int width, const UnqualifiedTypePtr& t, const Meta& meta = {}) {
    return hilti::ctor::UnsignedInteger::create(context(), value, width, t, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/integer.h:76:5
auto ctorValueReference(const ExpressionPtr& expr, const Meta& meta = {}) {
    return hilti::ctor::ValueReference::create(context(), expr, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/reference.h:64:5
auto ctorVector(Expressions exprs, const Meta& meta = {}) {
    return hilti::ctor::Vector::create(context(), exprs, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/vector.h:28:5
auto ctorVector(const QualifiedTypePtr& etype, Expressions exprs, const Meta& meta = {}) {
    return hilti::ctor::Vector::create(context(), etype, exprs, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/vector.h:23:5
auto ctorWeakReference(const QualifiedTypePtr& t, const Meta& meta = {}) {
    return hilti::ctor::WeakReference::create(context(), t, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/ctors/reference.h:44:5
auto declarationConstant(ID id, const ExpressionPtr& value,
                         declaration::Linkage linkage = declaration::Linkage::Private, Meta meta = {}) {
    return hilti::declaration::Constant::create(context(), id, value, linkage, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/declarations/constant.h:36:5
auto declarationConstant(ID id, const QualifiedTypePtr& type, const ExpressionPtr& value,
                         declaration::Linkage linkage = declaration::Linkage::Private, Meta meta = {}) {
    return hilti::declaration::Constant::create(context(), id, type, value, linkage, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/declarations/constant.h:31:5
auto declarationExpression(ID id, const ExpressionPtr& expr, const AttributeSetPtr& attrs, declaration::Linkage linkage,
                           Meta meta = {}) {
    return hilti::declaration::Expression::create(context(), id, expr, attrs, linkage, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/declarations/expression.h:23:5
auto declarationExpression(ID id, const ExpressionPtr& expr, declaration::Linkage linkage, Meta meta = {}) {
    return hilti::declaration::Expression::create(context(), id, expr, linkage, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/declarations/expression.h:27:5
auto declarationField(ID id, ::hilti::function::CallingConvention cc, const type::FunctionPtr& ftype,
                      const AttributeSetPtr& attrs, Meta meta = {}) {
    return hilti::declaration::Field::create(context(), id, cc, ftype, attrs, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/declarations/field.h:66:5
auto declarationField(ID id, const QualifiedTypePtr& type, const AttributeSetPtr& attrs, Meta meta = {}) {
    return hilti::declaration::Field::create(context(), id, type, attrs, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/declarations/field.h:61:5
auto declarationField(const ID& id, const FunctionPtr& inline_func, const AttributeSetPtr& attrs, Meta meta = {}) {
    return hilti::declaration::Field::create(context(), id, inline_func, attrs, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/declarations/field.h:71:5
auto declarationFunction(const FunctionPtr& function, declaration::Linkage linkage = declaration::Linkage::Private,
                         Meta meta = {}) {
    return hilti::declaration::Function::create(context(), function, linkage, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/declarations/function.h:43:5
auto declarationGlobalVariable(ID id, ExpressionPtr init, declaration::Linkage linkage = declaration::Linkage::Private,
                               Meta meta = {}) {
    return hilti::declaration::GlobalVariable::create(context(), id, init, linkage, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/declarations/global-variable.h:51:5
auto declarationGlobalVariable(ID id, QualifiedTypePtr type, ExpressionPtr init = nullptr,
                               declaration::Linkage linkage = declaration::Linkage::Private, Meta meta = {}) {
    return hilti::declaration::GlobalVariable::create(context(), id, type, init, linkage, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/declarations/global-variable.h:41:5
auto declarationGlobalVariable(ID id, QualifiedTypePtr type, Expressions args, ExpressionPtr init = nullptr,
                               declaration::Linkage linkage = declaration::Linkage::Private, Meta meta = {}) {
    return hilti::declaration::GlobalVariable::create(context(), id, type, args, init, linkage, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/declarations/global-variable.h:34:5
auto declarationGlobalVariable(ID id, QualifiedTypePtr type,
                               declaration::Linkage linkage = declaration::Linkage::Private, Meta meta = {}) {
    return hilti::declaration::GlobalVariable::create(context(), id, type, linkage, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/declarations/global-variable.h:46:5
auto declarationGlobalVariable(ID id, declaration::Linkage linkage = declaration::Linkage::Private, Meta meta = {}) {
    return hilti::declaration::GlobalVariable::create(context(), id, linkage, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/declarations/global-variable.h:56:5
auto declarationImportedModule(ID id, const std::string& parse_extension, Meta meta = {}) {
    return hilti::declaration::ImportedModule::create(context(), id, parse_extension, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/declarations/imported-module.h:51:5
auto declarationImportedModule(ID id, const std::string& parse_extension, std::optional<ID> search_scope,
                               Meta meta = {}) {
    return hilti::declaration::ImportedModule::create(context(), id, parse_extension, search_scope, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/declarations/imported-module.h:56:5
auto declarationImportedModule(ID id, hilti::rt::filesystem::path path, Meta meta = {}) {
    return hilti::declaration::ImportedModule::create(context(), id, path, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/declarations/imported-module.h:62:5
auto declarationLocalVariable(ID id, ExpressionPtr init, Meta meta = {}) {
    return hilti::declaration::LocalVariable::create(context(), id, init, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/declarations/local-variable.h:49:5
auto declarationLocalVariable(ID id, Meta meta = {}) {
    return hilti::declaration::LocalVariable::create(context(), id, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/declarations/local-variable.h:53:5
auto declarationLocalVariable(ID id, QualifiedTypePtr type, ExpressionPtr init = nullptr, Meta meta = {}) {
    return hilti::declaration::LocalVariable::create(context(), id, type, init, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/declarations/local-variable.h:41:5
auto declarationLocalVariable(ID id, QualifiedTypePtr type, Expressions args, ExpressionPtr init = nullptr,
                              Meta meta = {}) {
    return hilti::declaration::LocalVariable::create(context(), id, type, args, init, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/declarations/local-variable.h:34:5
auto declarationLocalVariable(ID id, QualifiedTypePtr type, Meta meta = {}) {
    return hilti::declaration::LocalVariable::create(context(), id, type, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/declarations/local-variable.h:45:5
auto declarationModule(const ModulePtr& module_, Meta meta = {}) {
    return hilti::declaration::Module::create(context(), module_, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/declarations/module.h:21:5
auto declarationParameter(ID id, const QualifiedTypePtr& type, declaration::parameter::Kind kind,
                          const hilti::ExpressionPtr& default_, bool is_type_param, const AttributeSetPtr& attrs,
                          Meta meta = {}) {
    return hilti::declaration::Parameter::create(context(), id, type, kind, default_, is_type_param, attrs, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/declarations/parameter.h:72:5
auto declarationParameter(ID id, const QualifiedTypePtr& type, declaration::parameter::Kind kind,
                          const hilti::ExpressionPtr& default_, const AttributeSetPtr& attrs, Meta meta = {}) {
    return hilti::declaration::Parameter::create(context(), id, type, kind, default_, attrs, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/declarations/parameter.h:66:5
auto declarationProperty(ID id, Meta meta = {}) {
    return hilti::declaration::Property::create(context(), id, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/declarations/property.h:22:5
auto declarationProperty(ID id, const ExpressionPtr& expr, Meta meta = {}) {
    return hilti::declaration::Property::create(context(), id, expr, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/declarations/property.h:26:5
auto declarationType(ID id, const UnqualifiedTypePtr& type, const AttributeSetPtr& attrs,
                     declaration::Linkage linkage = declaration::Linkage::Private, Meta meta = {}) {
    return hilti::declaration::Type::create(context(), id, type, attrs, linkage, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/declarations/type.h:41:5
auto declarationType(ID id, const UnqualifiedTypePtr& type,
                     declaration::Linkage linkage = declaration::Linkage::Private, Meta meta = {}) {
    return hilti::declaration::Type::create(context(), id, type, linkage, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/declarations/type.h:46:5
auto expressionAssign(const ExpressionPtr& target, const ExpressionPtr& src, const Meta& meta = {}) {
    return hilti::expression::Assign::create(context(), target, src, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/expressions/assign.h:23:5
auto expressionBuiltInFunction(const std::string& name, const std::string& cxxname, const QualifiedTypePtr& type,
                               const type::function::Parameters& parameters, const Expressions& arguments,
                               const Meta& meta = {}) {
    return hilti::expression::BuiltInFunction::create(context(), name, cxxname, type, parameters, arguments, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/expressions/builtin-function.h:42:5
auto expressionCoerced(const ExpressionPtr& expr, const QualifiedTypePtr& target, const Meta& meta = {}) {
    return hilti::expression::Coerced::create(context(), expr, target, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/expressions/coerced.h:22:5
auto expressionCtor(const CtorPtr& ctor, const Meta& meta = {}) {
    return hilti::expression::Ctor::create(context(), ctor, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/expressions/ctor.h:23:5
auto expressionDeferred(const ExpressionPtr& expr, bool catch_exception, const Meta& meta = {}) {
    return hilti::expression::Deferred::create(context(), expr, catch_exception, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/expressions/deferred.h:33:5
auto expressionDeferred(const ExpressionPtr& expr, const Meta& meta = {}) {
    return hilti::expression::Deferred::create(context(), expr, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/expressions/deferred.h:38:5
auto expressionGrouping(const ExpressionPtr& expr, const Meta& meta = {}) {
    return hilti::expression::Grouping::create(context(), expr, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/expressions/grouping.h:22:5
auto expressionKeyword(expression::keyword::Kind kind, const Meta& meta = {}) {
    return hilti::expression::Keyword::create(context(), kind, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/expressions/keyword.h:59:5
auto expressionKeyword(expression::keyword::Kind kind, const QualifiedTypePtr& type, const Meta& meta = {}) {
    return hilti::expression::Keyword::create(context(), kind, type, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/expressions/keyword.h:55:5
auto expressionListComprehension(const ExpressionPtr& input, const ExpressionPtr& output, const ID& id,
                                 const ExpressionPtr& cond, const Meta& meta = {}) {
    return hilti::expression::ListComprehension::create(context(), input, output, id, cond, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/expressions/list-comprehension.h:34:5
auto expressionLogicalAnd(const ExpressionPtr& op0, const ExpressionPtr& op1, const Meta& meta = {}) {
    return hilti::expression::LogicalAnd::create(context(), op0, op1, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/expressions/logical-and.h:24:5
auto expressionLogicalNot(const ExpressionPtr& expression, const Meta& meta = {}) {
    return hilti::expression::LogicalNot::create(context(), expression, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/expressions/logical-not.h:23:5
auto expressionLogicalOr(const ExpressionPtr& op0, const ExpressionPtr& op1, const Meta& meta = {}) {
    return hilti::expression::LogicalOr::create(context(), op0, op1, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/expressions/logical-or.h:24:5
auto expressionMember(const QualifiedTypePtr& member_type, const hilti::ID& id, const Meta& meta = {}) {
    return hilti::expression::Member::create(context(), member_type, id, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/expressions/member.h:29:5
auto expressionMember(const hilti::ID& id, const Meta& meta = {}) {
    return hilti::expression::Member::create(context(), id, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/expressions/member.h:34:5
auto expressionMove(const ExpressionPtr& expression, const Meta& meta = {}) {
    return hilti::expression::Move::create(context(), expression, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/expressions/move.h:23:5
auto expressionName(const hilti::ID& id, const Meta& meta = {}) {
    return hilti::expression::Name::create(context(), id, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/expressions/id.h:32:5
auto expressionPendingCoerced(const ExpressionPtr& expr, const QualifiedTypePtr& type, const Meta& meta = {}) {
    return hilti::expression::PendingCoerced::create(context(), expr, type, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/expressions/pending-coerced.h:25:5
auto expressionTernary(const ExpressionPtr& cond, const ExpressionPtr& true_, const ExpressionPtr& false_,
                       const Meta& meta = {}) {
    return hilti::expression::Ternary::create(context(), cond, true_, false_, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/expressions/ternary.h:29:5
auto expressionType(const QualifiedTypePtr& type, const Meta& meta = {}) {
    return hilti::expression::Type_::create(context(), type, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/expressions/type.h:23:5
auto expressionTypeInfo(const ExpressionPtr& expr, const Meta& meta = {}) {
    return hilti::expression::TypeInfo::create(context(), expr, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/expressions/typeinfo.h:22:5
auto expressionTypeWrapped(const ExpressionPtr& expr, const QualifiedTypePtr& type, const Meta& meta = {}) {
    return hilti::expression::TypeWrapped::create(context(), expr, type, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/expressions/type-wrapped.h:22:5
auto expressionUnresolvedOperator(operator_::Kind kind, Expressions operands, const Meta& meta = {}) {
    return hilti::expression::UnresolvedOperator::create(context(), kind, operands, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/expressions/unresolved-operator.h:33:5
auto expressionUnresolvedOperator(operator_::Kind kind, hilti::node::Range<Expression> operands,
                                  const Meta& meta = {}) {
    return hilti::expression::UnresolvedOperator::create(context(), kind, operands, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/expressions/unresolved-operator.h:39:5
auto expressionVoid(const Meta& meta = {}) {
    return hilti::expression::Void::create(context(), meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/expressions/void.h:21:5
auto function(const ID& id, const type::FunctionPtr& ftype, const StatementPtr& body,
              function::CallingConvention cc = function::CallingConvention::Standard,
              const AttributeSetPtr& attrs = nullptr, const Meta& meta = {}) {
    return hilti::Function::create(context(), id, ftype, body, cc, attrs, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/function.h:66:5
auto module(const module::UID& uid, const ID& scope = {}, const Meta& meta = {}) {
    return hilti::Module::create(context(), uid, scope, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/module.h:142:5
auto module(const module::UID& uid, const ID& scope, const Declarations& decls, Statements stmts,
            const Meta& meta = {}) {
    return hilti::Module::create(context(), uid, scope, decls, stmts, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/module.h:133:5
auto module(const module::UID& uid, const ID& scope, const Declarations& decls, const Meta& meta = {}) {
    return hilti::Module::create(context(), uid, scope, decls, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/module.h:146:5
auto qualifiedType(const UnqualifiedTypePtr& t, bool is_constant, Meta m = Meta()) {
    return hilti::QualifiedType::create(context(), t, is_constant, m);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/type.h:160:5
auto statementAssert(const ExpressionPtr& expr, const ExpressionPtr& msg = nullptr, Meta meta = {}) {
    return hilti::statement::Assert::create(context(), expr, msg, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/statements/assert.h:37:5
auto statementAssert(statement::assert::Exception _unused, const ExpressionPtr& expr, const UnqualifiedTypePtr& excpt,
                     const ExpressionPtr& msg = nullptr, const Meta& meta = {}) {
    return hilti::statement::Assert::create(context(), _unused, expr, excpt, msg, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/statements/assert.h:51:5
auto statementBlock(Meta meta = {}) {
    return hilti::statement::Block::create(context(), meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/statements/block.h:23:5
auto statementBlock(Statements stmts = {}, Meta meta = {}) {
    return hilti::statement::Block::create(context(), stmts, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/statements/block.h:19:5
auto statementBreak(Meta meta = {}) {
    return hilti::statement::Break::create(context(), meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/statements/break.h:15:5
auto statementComment(std::string comment,
                      statement::comment::Separator separator = statement::comment::Separator::Before, Meta meta = {}) {
    return hilti::statement::Comment::create(context(), comment, separator, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/statements/comment.h:39:5
auto statementContinue(Meta meta = {}) {
    return hilti::statement::Continue::create(context(), meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/statements/continue.h:17:5
auto statementDeclaration(const hilti::DeclarationPtr& d, Meta meta = {}) {
    return hilti::statement::Declaration::create(context(), d, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/statements/declaration.h:18:5
auto statementExpression(const ExpressionPtr& e, Meta meta = {}) {
    return hilti::statement::Expression::create(context(), e, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/statements/expression.h:18:5
auto statementFor(const hilti::ID& id, const ExpressionPtr& seq, const StatementPtr& body, Meta meta = {}) {
    return hilti::statement::For::create(context(), id, seq, body, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/statements/for.h:21:5
auto statementIf(const DeclarationPtr& init, const ExpressionPtr& cond, const StatementPtr& true_,
                 const StatementPtr& false_, Meta meta = {}) {
    return hilti::statement::If::create(context(), init, cond, true_, false_, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/statements/if.h:27:5
auto statementIf(const ExpressionPtr& cond, const StatementPtr& true_, const StatementPtr& false_, Meta meta = {}) {
    return hilti::statement::If::create(context(), cond, true_, false_, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/statements/if.h:32:5
auto statementReturn(ExpressionPtr expr, Meta meta = {}) {
    return hilti::statement::Return::create(context(), expr, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/statements/return.h:18:5
auto statementReturn(Meta meta = {}) {
    return hilti::statement::Return::create(context(), meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/statements/return.h:22:5
auto statementSetLocation(const ExpressionPtr& expr, Meta meta = {}) {
    return hilti::statement::SetLocation::create(context(), expr, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/statements/set_location.h:17:5
auto statementSwitch(DeclarationPtr cond, const statement::switch_::Cases& cases, Meta meta = {}) {
    return hilti::statement::Switch::create(context(), cond, cases, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/statements/switch.h:117:5
auto statementSwitch(const ExpressionPtr& cond, const statement::switch_::Cases& cases, Meta meta = {}) {
    return hilti::statement::Switch::create(context(), cond, cases, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/statements/switch.h:121:5
auto statementSwitchCase(const ExpressionPtr& expr, const StatementPtr& body, Meta meta = {}) {
    return hilti::statement::switch_::Case::create(context(), expr, body, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/statements/switch.h:50:5
auto statementSwitchCase(const Expressions& exprs, const StatementPtr& body, Meta meta = {}) {
    return hilti::statement::switch_::Case::create(context(), exprs, body, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/statements/switch.h:46:5
auto statementSwitchCase(statement::switch_::Default _unused, const StatementPtr& body, Meta meta = {}) {
    return hilti::statement::switch_::Case::create(context(), _unused, body, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/statements/switch.h:54:5
auto statementThrow(Meta meta = {}) {
    return hilti::statement::Throw::create(context(), meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/statements/throw.h:22:5
auto statementThrow(const ExpressionPtr& expr, Meta meta = {}) {
    return hilti::statement::Throw::create(context(), expr, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/statements/throw.h:18:5
auto statementTry(StatementPtr body, const statement::try_::Catches& catches, Meta meta = {}) {
    return hilti::statement::Try::create(context(), body, catches, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/statements/try.h:59:5
auto statementTryCatch(const DeclarationPtr& param, const StatementPtr& body, Meta meta = {}) {
    return hilti::statement::try_::Catch::create(context(), param, body, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/statements/try.h:27:5
auto statementTryCatch(const StatementPtr& body, Meta meta = {}) {
    return hilti::statement::try_::Catch::create(context(), body, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/statements/try.h:31:5
auto statementWhile(const DeclarationPtr& init, const ExpressionPtr& cond, const StatementPtr& body,
                    const StatementPtr& else_ = nullptr, const Meta& meta = {}) {
    return hilti::statement::While::create(context(), init, cond, body, else_, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/statements/while.h:23:5
auto statementWhile(const ExpressionPtr& cond, const StatementPtr& body, const Meta& meta = {}) {
    return hilti::statement::While::create(context(), cond, body, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/statements/while.h:28:5
auto statementWhile(const ExpressionPtr& cond, const StatementPtr& body, const StatementPtr& else_ = nullptr,
                    const Meta& meta = {}) {
    return hilti::statement::While::create(context(), cond, body, else_, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/statements/while.h:31:5
auto statementYield(Meta meta = {}) {
    return hilti::statement::Yield::create(context(), meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/statements/yield.h:15:5
auto typeAddress(Meta m = Meta()) {
    return hilti::type::Address::create(context(), m);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/address.h:15:5
auto typeAny(Meta m = Meta()) {
    return hilti::type::Any::create(context(), m);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/any.h:15:5
auto typeBool(Meta meta = {}) {
    return hilti::type::Bool::create(context(), meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/bool.h:15:5
auto typeBytes(const Meta& meta = {}) {
    return hilti::type::Bytes::create(context(), meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/bytes.h:46:5
auto typeBytesIterator(Meta meta = {}) {
    return hilti::type::bytes::Iterator::create(context(), meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/bytes.h:21:5
auto typeDocOnly(std::string description, Meta meta = {}) {
    return hilti::type::DocOnly::create(context(), description, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/doc-only.h:22:5
auto typeEnum(type::Wildcard _, Meta m = Meta()) {
    return hilti::type::Enum::create(context(), _, m);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/enum.h:112:5
auto typeEnum(type::enum_::Labels labels, Meta meta = {}) {
    return hilti::type::Enum::create(context(), labels, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/enum.h:108:5
auto typeEnumLabel(const ID& id, const Meta& meta = {}) {
    return hilti::type::enum_::Label::create(context(), id, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/enum.h:40:5
auto typeEnumLabel(const ID& id, int value, const Meta& meta = {}) {
    return hilti::type::enum_::Label::create(context(), id, value, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/enum.h:36:5
auto typeError(Meta meta = {}) {
    return hilti::type::Error::create(context(), meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/error.h:15:5
auto typeException(const Meta& meta = {}) {
    return hilti::type::Exception::create(context(), meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/exception.h:25:5
auto typeException(const UnqualifiedTypePtr& base, Meta meta = {}) {
    return hilti::type::Exception::create(context(), base, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/exception.h:21:5
auto typeException(type::Wildcard _, Meta m = Meta()) {
    return hilti::type::Exception::create(context(), _, m);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/exception.h:27:5
auto typeFunction(const QualifiedTypePtr& result, const declaration::Parameters& params,
                  type::function::Flavor flavor = type::function::Flavor::Standard, const Meta& meta = {}) {
    return hilti::type::Function::create(context(), result, params, flavor, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/function.h:74:5
auto typeFunction(type::Wildcard _, Meta m = Meta()) {
    return hilti::type::Function::create(context(), _, m);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/function.h:79:5
auto typeInterval(Meta meta = {}) {
    return hilti::type::Interval::create(context(), meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/interval.h:14:5
auto typeLibrary(const std::string& cxx_name, Meta meta = {}) {
    return hilti::type::Library::create(context(), cxx_name, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/library.h:29:5
auto typeList(const QualifiedTypePtr& t, const Meta& meta = {}) {
    return hilti::type::List::create(context(), t, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/list.h:53:5
auto typeList(type::Wildcard _, Meta m = Meta()) {
    return hilti::type::List::create(context(), _, m);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/list.h:57:5
auto typeListIterator(const QualifiedTypePtr& etype, Meta meta = {}) {
    return hilti::type::list::Iterator::create(context(), etype, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/list.h:20:5
auto typeListIterator(type::Wildcard _, Meta m = Meta()) {
    return hilti::type::list::Iterator::create(context(), _, m);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/list.h:24:5
auto typeMap(const QualifiedTypePtr& ktype, const QualifiedTypePtr& vtype, const Meta& meta = {}) {
    return hilti::type::Map::create(context(), ktype, vtype, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/map.h:76:5
auto typeMap(type::Wildcard _, Meta m = Meta()) {
    return hilti::type::Map::create(context(), _, m);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/map.h:81:5
auto typeMapIterator(const QualifiedTypePtr& ktype, const QualifiedTypePtr& vtype, Meta meta = {}) {
    return hilti::type::map::Iterator::create(context(), ktype, vtype, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/map.h:38:5
auto typeMapIterator(type::Wildcard _, Meta m = Meta()) {
    return hilti::type::map::Iterator::create(context(), _, m);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/map.h:42:5
auto typeMember(const ID& id, Meta meta = {}) {
    return hilti::type::Member::create(context(), id, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/member.h:23:5
auto typeMember(type::Wildcard _, Meta m = Meta()) {
    return hilti::type::Member::create(context(), _, m);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/member.h:27:5
auto typeName(ID id, Meta meta = {}) {
    return hilti::type::Name::create(context(), id, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/unresolved-id.h:16:5
auto typeNetwork(Meta meta = {}) {
    return hilti::type::Network::create(context(), meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/network.h:14:5
auto typeNull(Meta meta = {}) {
    return hilti::type::Null::create(context(), meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/null.h:18:5
auto typeOperandList(operator_::Operands operands, Meta meta = {}) {
    return hilti::type::OperandList::create(context(), operands, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/operand-list.h:40:5
auto typeOperandList(type::Wildcard _, Meta m = Meta()) {
    return hilti::type::OperandList::create(context(), _, m);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/operand-list.h:44:5
auto typeOptional(const QualifiedTypePtr& t, Meta m = Meta()) {
    return hilti::type::Optional::create(context(), t, m);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/optional.h:18:5
auto typeOptional(type::Wildcard _, Meta m = Meta()) {
    return hilti::type::Optional::create(context(), _, m);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/optional.h:22:5
auto typePort(Meta meta = {}) {
    return hilti::type::Port::create(context(), meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/port.h:14:5
auto typeReal(Meta meta = {}) {
    return hilti::type::Real::create(context(), meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/real.h:14:5
auto typeRegExp(Meta meta = {}) {
    return hilti::type::RegExp::create(context(), meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/regexp.h:14:5
auto typeResult(const QualifiedTypePtr& t, Meta m = Meta()) {
    return hilti::type::Result::create(context(), t, m);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/result.h:18:5
auto typeResult(type::Wildcard _, Meta m = Meta()) {
    return hilti::type::Result::create(context(), _, m);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/result.h:22:5
auto typeSet(const QualifiedTypePtr& t, const Meta& meta = {}) {
    return hilti::type::Set::create(context(), t, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/set.h:54:5
auto typeSet(type::Wildcard _, Meta m = Meta()) {
    return hilti::type::Set::create(context(), _, m);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/set.h:58:5
auto typeSetIterator(const QualifiedTypePtr& etype, Meta meta = {}) {
    return hilti::type::set::Iterator::create(context(), etype, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/set.h:21:5
auto typeSetIterator(type::Wildcard _, Meta m = Meta()) {
    return hilti::type::set::Iterator::create(context(), _, m);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/set.h:25:5
auto typeSignedInteger(int width, const Meta& m = Meta()) {
    return hilti::type::SignedInteger::create(context(), width, m);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/integer.h:46:5
auto typeSignedInteger(type::Wildcard _, Meta m = Meta()) {
    return hilti::type::SignedInteger::create(context(), _, m);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/integer.h:48:5
auto typeStream(const Meta& meta = {}) {
    return hilti::type::Stream::create(context(), meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/stream.h:72:5
auto typeStreamIterator(Meta meta = {}) {
    return hilti::type::stream::Iterator::create(context(), meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/stream.h:23:5
auto typeStreamView(Meta meta = {}) {
    return hilti::type::stream::View::create(context(), meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/stream.h:48:5
auto typeString(Meta meta = {}) {
    return hilti::type::String::create(context(), meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/string.h:15:5
auto typeStrongReference(const QualifiedTypePtr& type, Meta meta = {}) {
    return hilti::type::StrongReference::create(context(), type, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/reference.h:21:5
auto typeStrongReference(type::Wildcard _, Meta m = Meta()) {
    return hilti::type::StrongReference::create(context(), _, m);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/reference.h:25:5
auto typeStruct(const Declarations& fields, const Meta& meta = {}) {
    return hilti::type::Struct::create(context(), fields, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/struct.h:67:5
auto typeStruct(const declaration::Parameters& params, Declarations fields, const Meta& meta = {}) {
    return hilti::type::Struct::create(context(), params, fields, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/struct.h:59:5
auto typeStruct(type::Struct::AnonymousStruct _, Declarations fields, const Meta& meta = {}) {
    return hilti::type::Struct::create(context(), _, fields, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/struct.h:72:5
auto typeStruct(type::Wildcard _, const Meta& meta = {}) {
    return hilti::type::Struct::create(context(), _, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/struct.h:76:5
auto typeTime(Meta meta = {}) {
    return hilti::type::Time::create(context(), meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/time.h:14:5
auto typeTuple(const QualifiedTypes& types, const Meta& meta = {}) {
    return hilti::type::Tuple::create(context(), types, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/tuple.h:76:5
auto typeTuple(type::Wildcard _, Meta m = Meta()) {
    return hilti::type::Tuple::create(context(), _, m);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/tuple.h:81:5
auto typeTuple(type::tuple::Elements elements, Meta meta = {}) {
    return hilti::type::Tuple::create(context(), elements, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/tuple.h:72:5
auto typeTupleElement(ID id, const QualifiedTypePtr& type, Meta meta = {}) {
    return hilti::type::tuple::Element::create(context(), id, type, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/tuple.h:32:5
auto typeTupleElement(const QualifiedTypePtr& type, Meta meta = {}) {
    return hilti::type::tuple::Element::create(context(), type, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/tuple.h:36:5
auto typeType(const QualifiedTypePtr& type, Meta meta = {}) {
    return hilti::type::Type_::create(context(), type, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/type.h:19:5
auto typeType(type::Wildcard _, Meta m = Meta()) {
    return hilti::type::Type_::create(context(), _, m);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/type.h:23:5
auto typeUnion(const Declarations& fields, const Meta& meta = {}) {
    return hilti::type::Union::create(context(), fields, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/union.h:67:5
auto typeUnion(const declaration::Parameters& params, Declarations fields, const Meta& meta = {}) {
    return hilti::type::Union::create(context(), params, fields, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/union.h:59:5
auto typeUnion(type::Union::AnonymousUnion _, Declarations fields, const Meta& meta = {}) {
    return hilti::type::Union::create(context(), _, fields, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/union.h:72:5
auto typeUnion(type::Wildcard _, const Meta& meta = {}) {
    return hilti::type::Union::create(context(), _, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/union.h:76:5
auto typeUnknown(Meta meta = {}) {
    return hilti::type::Unknown::create(context(), meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/unknown.h:14:5
auto typeUnsignedInteger(int width, const Meta& m = Meta()) {
    return hilti::type::UnsignedInteger::create(context(), width, m);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/integer.h:68:5
auto typeUnsignedInteger(type::Wildcard _, Meta m = Meta()) {
    return hilti::type::UnsignedInteger::create(context(), _, m);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/integer.h:70:5
auto typeValueReference(const QualifiedTypePtr& type, Meta meta = {}) {
    return hilti::type::ValueReference::create(context(), type, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/reference.h:85:5
auto typeValueReference(type::Wildcard _, Meta m = Meta()) {
    return hilti::type::ValueReference::create(context(), _, m);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/reference.h:89:5
auto typeVector(const QualifiedTypePtr& t, const Meta& meta = {}) {
    return hilti::type::Vector::create(context(), t, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/vector.h:53:5
auto typeVector(type::Wildcard _, Meta m = Meta()) {
    return hilti::type::Vector::create(context(), _, m);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/vector.h:57:5
auto typeVectorIterator(const QualifiedTypePtr& etype, Meta meta = {}) {
    return hilti::type::vector::Iterator::create(context(), etype, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/vector.h:20:5
auto typeVectorIterator(type::Wildcard _, Meta m = Meta()) {
    return hilti::type::vector::Iterator::create(context(), _, m);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/vector.h:24:5
auto typeVoid(Meta meta = {}) {
    return hilti::type::Void::create(context(), meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/void.h:14:5
auto typeWeakReference(const QualifiedTypePtr& type, Meta meta = {}) {
    return hilti::type::WeakReference::create(context(), type, meta);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/reference.h:53:5
auto typeWeakReference(type::Wildcard _, Meta m = Meta()) {
    return hilti::type::WeakReference::create(context(), _, m);
} // /Users/robin/work/spicy/node-rewrite/hilti/toolchain/include/hilti/ast/types/reference.h:57:5
