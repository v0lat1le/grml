#pragma once

#include <boost/variant.hpp>

namespace grml
{
    enum class BasicType
    {
        INT,
        BOOL,
        REAL
    };

    struct TypeVariable
    {
        friend bool operator==(const TypeVariable&, const TypeVariable&)
        {
            return false;
        }
    };

    using Type = boost::variant<BasicType, TypeVariable>;
}
