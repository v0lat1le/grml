#pragma once

#include "types.h"
#include "ast.h"

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
    using Environment = std::unordered_map<Identifier, Type, detail::IdHasher>;

    Type infer(const Expression& expr, const Environment& env = Environment());
}
