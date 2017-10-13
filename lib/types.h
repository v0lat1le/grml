#pragma once

#include <boost/variant.hpp>
#include <atomic>
#include <vector>

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
        int id;

        TypeVariable() : id(counter++) {}
        TypeVariable(int i) : id(i) {}

        friend bool operator==(const TypeVariable& lhs, const TypeVariable& rhs)
        {
            return lhs.id == rhs.id;
        }
    private:
        inline static std::atomic_int64_t counter{0};
    };

    struct FunctionType;

    using Type = boost::variant<
        BasicType,
        TypeVariable,
        boost::recursive_wrapper<FunctionType>
    >;

    struct FunctionType
    {
        using Parameters = std::vector<Type>;

        Type result;
        Parameters parameters;

        FunctionType(Type r, Parameters ps) : result(std::move(r)), parameters(std::move(ps)) {}

        friend bool operator==(const FunctionType& lhs, const FunctionType& rhs)
        {
            return lhs.result == rhs.result && lhs.parameters == rhs.parameters;
        }
    };
}
