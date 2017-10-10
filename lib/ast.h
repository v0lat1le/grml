#pragma once

#include <boost/variant.hpp>

namespace grml
{
    enum class UnaryOperator
    {
        NOT,
        PLUS,
        MINUS,
    };

    enum class BinaryOperator
    {
        ADD,      SUBTRACT,
        MULTIPLY, DIVIDE,
        MODULO,
        LEFT_SHIFT, RIGHT_SHIFT,
        EQUAL,      NOT_EQUAL,
        LOWER,      LOWER_EQUAL,
        GREATER,    GREATER_EQUAL,
    };

    using Literal = boost::variant<int, double, bool>;

    struct UnaryOperation;
    struct BinaryOperation;

    using Expression = boost::variant<
        Literal,
        boost::recursive_wrapper<UnaryOperation>,
        boost::recursive_wrapper<BinaryOperation>
    >;

    struct UnaryOperation
    {
        UnaryOperator op;
        Expression rhs;
        
        UnaryOperation() {}
        UnaryOperation(UnaryOperator o, Expression r) : op(o), rhs(r) {}
    };
    
    bool operator == (const UnaryOperation& lhs, const UnaryOperation& rhs)
    {
        return lhs.op == rhs.op && lhs.rhs == rhs.rhs;
    }

    struct BinaryOperation
    {
        BinaryOperator op;
        Expression lhs;
        Expression rhs;

        BinaryOperation() {}
        BinaryOperation(BinaryOperator o, Expression l, Expression r) : op(o), lhs(l), rhs(r) {}
    };

    bool operator == (const BinaryOperation& lhs, const BinaryOperation& rhs)
    {
        return lhs.op == rhs.op && lhs.lhs == rhs.lhs && lhs.rhs == rhs.rhs;
    }
}
