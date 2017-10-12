#define BOOST_TEST_MODULE my master test suite name
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
  BOOST_TEST(parse("5;") == grml::Expression(grml::Literal(5)));
  BOOST_TEST(parse("-5;") == grml::Expression(grml::Literal(-5)));
  BOOST_TEST(parse("false;") == grml::Expression(grml::Literal(false)));
  BOOST_TEST(parse("0.5;") == grml::Expression(grml::Literal(0.5)));

  BOOST_TEST(parse("3+5;") == grml::Expression(
    grml::BinaryOperation(grml::BinaryOperator::ADD, grml::Literal(3), grml::Literal(5))
  ));
  BOOST_TEST(parse("2*4;") == grml::Expression(
    grml::BinaryOperation(grml::BinaryOperator::MULTIPLY, grml::Literal(2), grml::Literal(4))
  ));
  BOOST_TEST(parse("2*4+6;") == grml::Expression(
    grml::BinaryOperation(grml::BinaryOperator::ADD, grml::BinaryOperation(grml::BinaryOperator::MULTIPLY, grml::Literal(2), grml::Literal(4)), grml::Literal(6))
  ));
  BOOST_TEST(parse("2+4*6;") == grml::Expression(
    grml::BinaryOperation(grml::BinaryOperator::ADD, grml::Literal(2), grml::BinaryOperation(grml::BinaryOperator::MULTIPLY, grml::Literal(4), grml::Literal(6)))
  ));
  BOOST_TEST(parse("2*(4+6);") == grml::Expression(
    grml::BinaryOperation(grml::BinaryOperator::MULTIPLY, grml::Literal(2), grml::BinaryOperation(grml::BinaryOperator::ADD, grml::Literal(4), grml::Literal(6)))
  ));
  BOOST_TEST(parse("(2+4)*6;") == grml::Expression(
    grml::BinaryOperation(grml::BinaryOperator::MULTIPLY, grml::BinaryOperation(grml::BinaryOperator::ADD, grml::Literal(2), grml::Literal(4)), grml::Literal(6))
  ));

  BOOST_TEST(parse("bob;") == grml::Expression(grml::Identifier("bob")));

  BOOST_TEST(parse("let in true end;") == grml::Expression(grml::LetConstruct({}, grml::Literal(true))));
  BOOST_TEST(parse("let val x = 5 in x - y end;") == grml::Expression(
    grml::LetConstruct(
      {grml::VariableDeclaration(grml::Identifier("x"), grml::Literal(5))},
      grml::BinaryOperation(grml::BinaryOperator::SUBTRACT, grml::Identifier("x"), grml::Identifier("y"))
    )
  ));

  BOOST_TEST(parse("let fun x(p) = 7 in true end;") == grml::Expression(
    grml::LetConstruct(
      {grml::FunctionDeclaration(grml::Identifier("x"), {grml::Identifier("p")}, grml::Literal(7))},
      grml::Literal(true)
    )
  ));
}
