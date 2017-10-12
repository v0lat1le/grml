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

    struct FunctionType;

    using Type = boost::variant<
        BasicType,
        TypeVariable,
        boost::recursive_wrapper<FunctionType>
    >;

    struct FunctionType
    {
        friend bool operator==(const FunctionType& lhs, const FunctionType& rhs)
        {
            return true;
        }
    };
}
