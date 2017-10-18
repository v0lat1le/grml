#define BOOST_TEST_MODULE parser test suite
#define BOOST_TEST_DYN_LINK

#include "parser.h"

#include <boost/test/unit_test.hpp>

#include <string_view>

BOOST_TEST_DONT_PRINT_LOG_VALUE(grml::Expression)

namespace
{
    grml::Expression parse(const std::string_view& str)
    {
        using boost::spirit::ascii::space;

        typedef std::string_view::const_iterator iterator_type;
        typedef grml::Parser<iterator_type> Parser;

        Parser parser;
        grml::Expression expr;
        
        bool r = phrase_parse(str.begin(), str.end(), parser, space, expr);
        BOOST_TEST_REQUIRE(r, "failed to parse \"" << str << "\"");

        return expr;
    }
}

BOOST_AUTO_TEST_CASE(test_expressions)
{
    using namespace grml;

    BOOST_TEST(parse("5;") == Expression(Literal(5)));
    BOOST_TEST(parse("-5;") == Expression(Literal(-5)));
    BOOST_TEST(parse("false;") == Expression(Literal(false)));
    BOOST_TEST(parse("0.5;") == Expression(Literal(0.5)));

    BOOST_TEST(parse("3+5;") == Expression(
        BinaryOperation(BinaryOperator::ADD, Literal(3), Literal(5))
    ));
    BOOST_TEST(parse("2*4;") == Expression(
        BinaryOperation(BinaryOperator::MULTIPLY, Literal(2), Literal(4))
    ));
    BOOST_TEST(parse("2*4+6;") == Expression(
        BinaryOperation(BinaryOperator::ADD,
            BinaryOperation(BinaryOperator::MULTIPLY, Literal(2), Literal(4)),
            Literal(6))
    ));
    BOOST_TEST(parse("2+4*6;") == Expression(
        BinaryOperation(BinaryOperator::ADD,
            Literal(2),
            BinaryOperation(BinaryOperator::MULTIPLY, Literal(4), Literal(6)))
    ));
    BOOST_TEST(parse("2*(4+6);") == Expression(
        BinaryOperation(BinaryOperator::MULTIPLY,
            Literal(2),
            BinaryOperation(BinaryOperator::ADD, Literal(4), Literal(6)))
    ));
    BOOST_TEST(parse("(2+4)*6;") == Expression(
        BinaryOperation(BinaryOperator::MULTIPLY,
            BinaryOperation(BinaryOperator::ADD, Literal(2), Literal(4)),
            Literal(6))
    ));

    BOOST_TEST(parse("bob;") == Expression(Identifier("bob")));

    BOOST_TEST(parse("let in true end;") == Expression(LetConstruct({}, Literal(true))));
    BOOST_TEST(
        parse("2 + let in true end;") ==
        Expression(BinaryOperation(BinaryOperator::ADD, Literal(2), LetConstruct({}, Literal(true))))
    );
    BOOST_TEST(
        parse("let in true end + 2;") ==
        Expression(BinaryOperation(BinaryOperator::ADD, LetConstruct({}, Literal(true)), Literal(2)))
    );
    BOOST_TEST(parse("let val x = 5 in x - y end;") == Expression(
        LetConstruct(
        { VariableDeclaration(Identifier("x"), Literal(5)) },
        BinaryOperation(BinaryOperator::SUBTRACT, Identifier("x"), Identifier("y"))
        )
    ));
    
    BOOST_TEST(parse("let fun x() = 7 in true end;") == Expression(
        LetConstruct(
        {FunctionDeclaration(Identifier("x"), {}, Literal(7))},
        Literal(true)
        )
    ));

    BOOST_TEST(parse("let fun x(p, k) = 7 in true end;") == Expression(
        LetConstruct(
        { FunctionDeclaration(Identifier("x"), { Identifier("p"), Identifier("k") }, Literal(7)) },
        Literal(true)
        )
    ));

    BOOST_TEST(parse("fun();") == Expression(FunctionCall(Identifier("fun"), {})));
    BOOST_TEST(
        parse("2 + fun();") ==
        Expression(BinaryOperation(BinaryOperator::ADD, Literal(2), FunctionCall(Identifier("fun"), {})))
    );
    BOOST_TEST(
        parse("fun() + 2;") ==
        Expression(BinaryOperation(BinaryOperator::ADD, FunctionCall(Identifier("fun"), {}), Literal(2)))
    );
    BOOST_TEST(
        parse("fun(1, true, 0.5, x);") ==
        Expression(FunctionCall(Identifier("fun"), {Literal(1), Literal(true), Literal(0.5), Identifier("x") }))
    );

    BOOST_TEST(parse("if true then 4 else 5;") == Expression(IfConstruct(Literal(true), Literal(4), Literal(5))));
}
