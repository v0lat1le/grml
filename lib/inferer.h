#pragma once

#include "types.h"
#include "ast.h"

#include <unordered_set>
#include <unordered_map>


namespace grml
{
    namespace detail
    {
        struct IdHasher
        {
            std::size_t operator()(const Identifier& id) const;
        };
    }

    struct TypeSchema
    {
        using FreeVars = std::unordered_set<TypeVariable, TypeVariableHasher>;
        Type type;
        FreeVars freevars;
    };

    using Environment = std::unordered_map<Identifier, TypeSchema, detail::IdHasher>;

    Type instantiate(const TypeSchema& type);
    Type infer(const Expression& expr, const Environment& env = Environment());
}
