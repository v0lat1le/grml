#define BOOST_TEST_MODULE inferer test suite
#define BOOST_TEST_DYN_LINK

#include "inferer.h"

#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(test_inferer)
{
    using namespace grml;

    BOOST_TEST(infer(Literal(5)) == Type(BasicType::INT));
    BOOST_TEST(infer(Literal(false)) == Type(BasicType::BOOL));
    BOOST_TEST(infer(Literal(0.5)) == Type(BasicType::REAL));

    auto letx5 = Expression(
        LetConstruct(
            { VariableDeclaration(Identifier("x"), Literal(5)) },
            Identifier("x")
        )
    );
    BOOST_TEST(infer(letx5) == Type(BasicType::INT));

    auto add35 = Expression(
        BinaryOperation(BinaryOperator::ADD, Literal(3), Literal(5))
    );
    BOOST_TEST(infer(add35) == Type(BasicType::INT));

    auto letfun = Expression(
        LetConstruct(
            { FunctionDeclaration(Identifier("x"), { Identifier("p") }, Literal(5)) },
            Identifier("x")
        )
    );
    BOOST_TEST(infer(letfun) == Type(FunctionType(BasicType::INT, { TypeVariable(0) })));

    auto funcall = Expression(
        LetConstruct(
            { FunctionDeclaration(Identifier("f"), { Identifier("p") }, Identifier("p")) },
            FunctionCall(Identifier("f"), { Literal(5) })
        )
    );
    BOOST_TEST(infer(funcall) == Type(BasicType::INT));

    auto funcall2 = Expression(
        LetConstruct(
            {
                FunctionDeclaration(Identifier("f"), { Identifier("p") }, Identifier("p")),
                FunctionDeclaration(Identifier("g"), {
                    Identifier("p") }, FunctionCall(Identifier("f"), { Identifier("p")
                }))
            },
            FunctionCall(Identifier("g"), { Literal(5) })
        )
    );
    BOOST_TEST(infer(funcall2) == Type(BasicType::INT));

    auto ifsimple = Expression(IfConstruct(Literal(true), Literal(4), Literal(5)));
    BOOST_TEST(infer(ifsimple) == Type(BasicType::INT));

    auto ifbadtest = Expression(IfConstruct(Literal(4), Literal(4), Literal(5)));
    BOOST_CHECK_THROW(infer(ifbadtest), std::runtime_error);

    auto ifbadbody = Expression(IfConstruct(Literal(true), Literal(4.5), Literal(5)));
    BOOST_CHECK_THROW(infer(ifbadbody), std::runtime_error);

    auto ifdeduceall = Expression(IfConstruct(Identifier("p"), Identifier("p"), Identifier("p")));
    BOOST_TEST(infer(ifdeduceall, {{Identifier("p"), TypeVariable()}}) == Type(BasicType::BOOL));
}
