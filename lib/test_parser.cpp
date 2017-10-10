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
    BOOST_TEST(r);

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
}
