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
    struct FunctionCall;
    struct IfConstruct;

    using Expression = boost::variant<
        Literal,
        Identifier,
        boost::recursive_wrapper<UnaryOperation>,
        boost::recursive_wrapper<BinaryOperation>,
        boost::recursive_wrapper<LetConstruct>,
        boost::recursive_wrapper<FunctionCall>,
        boost::recursive_wrapper<IfConstruct>
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
        Identifier name;
        Expression expression;

        VariableDeclaration() {}
        VariableDeclaration(Identifier n, Expression e) : name(std::move(n)), expression(std::move(e)) {}
    
        friend bool operator == (const VariableDeclaration& lhs, const VariableDeclaration& rhs)
        {
            return lhs.name == rhs.name && lhs.expression == rhs.expression;
        }
    };

    struct FunctionDeclaration
    {
        Identifier name;
        std::vector<Identifier> parameters;
        Expression expression;
        
        FunctionDeclaration() {}
        FunctionDeclaration(Identifier n, std::vector<Identifier> ps, Expression e) : name(std::move(n)), parameters(std::move(ps)), expression(std::move(e)) {}

        friend bool operator == (const FunctionDeclaration& lhs, const FunctionDeclaration& rhs)
        {
            return lhs.name == rhs.name && lhs.parameters == rhs.parameters && lhs.expression == rhs.expression;
        }
    };

    using Declaration = boost::variant<VariableDeclaration, FunctionDeclaration>;

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

    struct FunctionCall
    {
        Identifier name;
        std::vector<Expression> arguments;

        FunctionCall() {}
        FunctionCall(Identifier n, std::vector<Expression> args) : name(std::move(n)), arguments(std::move(args)) {}
        
        friend bool operator == (const FunctionCall& lhs, const FunctionCall& rhs)
        {
            return lhs.name == rhs.name && lhs.arguments == rhs.arguments;
        }
    };

    struct IfConstruct
    {
        Expression test;
        Expression whenTrue;
        Expression whenFalse;

        IfConstruct() {}
        IfConstruct(Expression c, Expression t, Expression f) : test(std::move(c)), whenTrue(std::move(t)), whenFalse(std::move(f)) {}
        
        friend bool operator == (const IfConstruct& lhs, const IfConstruct& rhs)
        {
            return lhs.test == rhs.test && lhs.whenTrue == rhs.whenTrue && lhs.whenFalse == rhs.whenFalse;
        }
    };
}
