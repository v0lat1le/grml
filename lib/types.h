#pragma once

#include <boost/variant.hpp>
#include <atomic>
#include <vector>
#include <unordered_map>

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
        int64_t id;

        TypeVariable() : id(counter++) {}
        TypeVariable(int64_t i) : id(i) {}

        friend bool operator==(const TypeVariable& lhs, const TypeVariable& rhs)
        {
            return lhs.id == rhs.id;
        }
    private:
        static std::atomic_int64_t counter;
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

    struct TypeVariableHasher
    {
        std::size_t operator()(const TypeVariable& tv) const
        {
            return tv.id;
        }
    };

    using Substitution = std::unordered_map<TypeVariable, Type, TypeVariableHasher>;

    Substitution unify(const Type& lhs, const Type& rhs);
    Substitution combine(const Substitution& lhs, const Substitution& rhs);
    Type substitute(const Type& type, const Substitution& substitution);
}
