// Copyright (c) 2020-2023 by the Zeek Project. See LICENSE for details.

#pragma once

#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <utility>

#include <hilti/ast/meta.h>
#include <hilti/base/id-base.h>
#include <hilti/base/util.h>

namespace hilti {

class ID : public detail::IDBase<ID> {
public:
    using IDBase::IDBase;

    ID(const char* s, Meta meta) : IDBase(s), _meta(std::move(meta)) {}
    explicit ID(std::string s, Meta meta) : IDBase(std::move(s)), _meta(std::move(meta)) {}

    const auto& meta() const { return _meta; }
    const auto& location() const { return _meta ? _meta->location() : location::None; }

private:
    std::optional<Meta> _meta;
};

inline std::ostream& operator<<(std::ostream& out, const ID& id) {
    out << std::string(id);
    return out;
}

} // namespace hilti

namespace std {
template<>
struct hash<hilti::ID> {
    std::size_t operator()(const hilti::ID& id) const { return hash<std::string>()(id); }
};
} // namespace std
