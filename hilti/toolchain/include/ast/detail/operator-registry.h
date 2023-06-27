// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <map>
#include <memory>
#include <vector>

#include <hilti/ast/operator.h>

#define REGISTER_OPERATOR(cls)                                                                                         \
    namespace {                                                                                                        \
    static hilti::operator_::Register<cls> _operator##cls;                                                             \
    }

namespace hilti::operator_ {

/** Singleton registering available operators. */
class Registry {
public:
    /** Returns a map of all available operators. */
    const auto& byKind(Kind kind) const { return _operators_by_kind.at(kind); }
    const auto& byMethodID(const ID& id) const { return _operators_by_method.at(id); }

    /** Registers an Operator as available. */
    void register_(std::unique_ptr<Operator> op);

    void init(ASTContext* ctx);

    /** Returns a singleton instance of the current class.  */
    static auto& singleton() {
        static Registry instance;
        return instance;
    }

private:
    std::vector<std::unique_ptr<Operator>> _operators;
    std::map<Kind, std::vector<Operator*>> _operators_by_kind;
    std::map<ID, std::vector<Operator*>> _operators_by_method;
};

/** Helper class to register an operator on instantiation. */
template<typename T>
class Register {
public:
    Register() { Registry::singleton().register_(std::make_unique<T>()); }
};

inline auto& registry() { return Registry::singleton(); }

} // namespace hilti::operator_
