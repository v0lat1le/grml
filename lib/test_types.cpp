#define BOOST_TEST_MODULE types test suite
#define BOOST_TEST_DYN_LINK

#include "types.h"

#include <boost/test/unit_test.hpp>

BOOST_TEST_DONT_PRINT_LOG_VALUE(grml::Type)

BOOST_AUTO_TEST_CASE(test_substitute)
{
    auto a = grml::TypeVariable();

    BOOST_TEST(grml::substitute(grml::BasicType::REAL, { { a, grml::BasicType::INT } } ) == grml::Type(grml::BasicType::REAL));
    BOOST_TEST(grml::substitute(a, { { a, grml::BasicType::INT } } ) == grml::Type(grml::BasicType::INT));
    BOOST_TEST(grml::substitute(a, {}) == grml::Type(a));

    BOOST_TEST(grml::substitute(grml::FunctionType(a, { a }), { { a, grml::BasicType::INT } }) ==
        grml::Type(grml::FunctionType(grml::BasicType::INT, { grml::BasicType::INT }))
    );
}

BOOST_AUTO_TEST_CASE(test_unify)
{
    grml::TypeVariable a, b;

    BOOST_TEST(grml::unify(a, a) == grml::Substitution());
    BOOST_TEST(grml::unify(a, b) == (grml::Substitution{ { a, b } }));
    BOOST_TEST(grml::unify(a, grml::BasicType::INT) == (grml::Substitution{ { a, grml::BasicType::INT } }));
    BOOST_TEST(grml::unify(grml::BasicType::BOOL, b) == grml::Substitution());

    BOOST_TEST(grml::unify(grml::FunctionType(a, { a }), grml::FunctionType(b, { grml::BasicType::INT })) ==
        (grml::Substitution{ { a, grml::BasicType::INT } })
    );
}
