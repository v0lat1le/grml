#define BOOST_TEST_MODULE types test suite
#define BOOST_TEST_DYN_LINK

#include "types.h"

#include <boost/test/unit_test.hpp>

BOOST_TEST_DONT_PRINT_LOG_VALUE(grml::Type)

BOOST_AUTO_TEST_CASE(test_substitute)
{
    auto tv = grml::TypeVariable();
    BOOST_TEST(grml::substitute(grml::BasicType::REAL, {{tv, grml::BasicType::INT}}) == grml::Type(grml::BasicType::REAL));
    BOOST_TEST(grml::substitute(tv, {{tv, grml::BasicType::INT}}) == grml::Type(grml::BasicType::INT));

    auto f = grml::FunctionType(grml::BasicType::INT, {grml::BasicType::INT});
    BOOST_TEST(grml::substitute(
        grml::FunctionType(tv, {tv}), {{tv, grml::BasicType::INT}}) ==
        grml::Type(grml::FunctionType(grml::BasicType::INT, {grml::BasicType::INT}))
    );
}