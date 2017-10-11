#define BOOST_TEST_MODULE inferer test suite
#define BOOST_TEST_DYN_LINK

#include "inferer.h"

#include <boost/test/unit_test.hpp>

BOOST_TEST_DONT_PRINT_LOG_VALUE(grml::Type)

BOOST_AUTO_TEST_CASE(test_inferer)
{
    BOOST_TEST(grml::infer(grml::Literal(5)) == grml::Type(grml::BasicType::INT));
    BOOST_TEST(grml::infer(grml::Literal(false)) == grml::Type(grml::BasicType::BOOL));
    BOOST_TEST(grml::infer(grml::Literal(0.5)) == grml::Type(grml::BasicType::REAL));
}