#pragma once

#include <boost/variant.hpp>
#include <vector>
#include <string>

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

    struct Identifier
    {
        std::string name;
        
        Identifier() {}
        Identifier(std::string n) : name(std::move(n)) {}
    
        friend bool operator == (const Identifier& lhs, const Identifier& rhs)
        {
            return lhs.name == rhs.name;
        }
    };

    struct UnaryOperation;
    struct BinaryOperation;
    struct LetConstruct;

    using Expression = boost::variant<
        Literal,
        Identifier,
        boost::recursive_wrapper<UnaryOperation>,
        boost::recursive_wrapper<BinaryOperation>,
        boost::recursive_wrapper<LetConstruct>
    >;

    struct UnaryOperation
    {
        UnaryOperator op;
        Expression rhs;
        
        UnaryOperation() {}
        UnaryOperation(UnaryOperator o, Expression r) : op(o), rhs(std::move(r)) {}
    
        friend bool operator == (const UnaryOperation& lhs, const UnaryOperation& rhs)
        {
            return lhs.op == rhs.op && lhs.rhs == rhs.rhs;
        }
    };

    struct BinaryOperation
    {
        BinaryOperator op;
        Expression lhs;
        Expression rhs;

        BinaryOperation() {}
        BinaryOperation(BinaryOperator o, Expression l, Expression r) : op(o), lhs(std::move(l)), rhs(std::move(r)) {}
    
        friend bool operator == (const BinaryOperation& lhs, const BinaryOperation& rhs)
        {
            return lhs.op == rhs.op && lhs.lhs == rhs.lhs && lhs.rhs == rhs.rhs;
        }
    };

    struct VariableDeclaration
    {
        Identifier identifier;
        Expression expression;

        VariableDeclaration() {}
        VariableDeclaration(Identifier id, Expression e) : identifier(std::move(id)), expression(std::move(e)) {}
    
        friend bool operator == (const VariableDeclaration& lhs, const VariableDeclaration& rhs)
        {
            return lhs.identifier == rhs.identifier && lhs.expression == rhs.expression;
        }
    };

    using Declaration = boost::variant<VariableDeclaration>;

    struct LetConstruct
    {
        std::vector<Declaration> declarations;
        Expression expression;

        LetConstruct() {}
        LetConstruct(std::vector<Declaration> ds, Expression e) : declarations(std::move(ds)), expression(std::move(e)) {}
        
        friend bool operator == (const LetConstruct& lhs, const LetConstruct& rhs)
        {
            return lhs.declarations == rhs.declarations && lhs.expression == rhs.expression;
        }
    };  
}
