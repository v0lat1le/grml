#define BOOST_TEST_MODULE inferer test suite
#define BOOST_TEST_DYN_LINK

#include "inferer.h"

#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(test_instantiate)
{
    using namespace grml;

    TypeVariable a, b, c, d;

    BOOST_TEST(instantiate({ FunctionType(BasicType::INT, { BasicType::REAL }), {} }) == Type(FunctionType(BasicType::INT, { BasicType::REAL })));

    auto faa = instantiate({ FunctionType(a, { a }), { a } });
    BOOST_TEST(boost::get<FunctionType>(faa).result == boost::get<FunctionType>(faa).parameters[0]);

    BOOST_TEST(instantiate({ FunctionType(a, { a }), {} }) == Type(FunctionType(a, { a })));
}

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
    auto letfunt = boost::get<FunctionType>(infer(letfun));
    BOOST_TEST(letfunt.result == Type(BasicType::INT));

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
    BOOST_TEST(infer(ifdeduceall, { { Identifier("p"), { TypeVariable(), {} } } }) == Type(BasicType::BOOL));

    auto iffunc1 = Expression(
        LetConstruct(
            {
                FunctionDeclaration(Identifier("f"), { Identifier("t"), Identifier("p") },
                 IfConstruct(Identifier("t"), Identifier("p"), Literal(5)))
            },
            Identifier("f"))
    );
    BOOST_TEST(infer(iffunc1) == Type(FunctionType(BasicType::INT, { BasicType::BOOL, BasicType::INT })));

    auto iffunc2 = Expression(
        LetConstruct(
            {
                FunctionDeclaration(Identifier("f"),{ Identifier("t"), Identifier("p") },
                IfConstruct(Identifier("t"), Literal(0.5), Identifier("p")))
            },
            Identifier("f"))
    );
    BOOST_TEST(infer(iffunc2) == Type(FunctionType(BasicType::REAL, { BasicType::BOOL, BasicType::REAL })));

    auto fakerecurse1 = Expression(
        LetConstruct(
            {
                FunctionDeclaration(Identifier("f"), { Identifier("g"), Identifier("x") },
                    IfConstruct(Literal(true),
                        FunctionCall(Identifier("g"), { Identifier("g"), Identifier("x")}),
                        Identifier("x")))
            },
            FunctionCall(Identifier("f"), { Identifier("f"), Literal(0.5) })
        )
    );
    BOOST_TEST(infer(fakerecurse1) == Type(BasicType::REAL));

    auto fakerecurse2 = Expression(
        LetConstruct(
            {
                FunctionDeclaration(Identifier("f"), { Identifier("g"), Identifier("x") },
                    IfConstruct(Literal(true),
                        Identifier("x"),
                        FunctionCall(Identifier("g"), { Identifier("g"), Identifier("x")})))
            },
            FunctionCall(Identifier("f"), { Identifier("f"), Literal(5) })
        )
    );
    BOOST_TEST(infer(fakerecurse2) == Type(BasicType::INT));

    auto funrecurse1 = Expression(
        LetConstruct(
            {
                FunctionDeclaration(Identifier("f"), { Identifier("p") },
                    IfConstruct(Literal(true),
                        FunctionCall(Identifier("f"), { Identifier("p")}),
                        Identifier("p")))
            },
            FunctionCall(Identifier("f"), { Literal(0.5) })
        )
    );
    BOOST_TEST(infer(funrecurse1) == Type(BasicType::REAL));

    auto funrecurse2 = Expression(
        LetConstruct(
            {
                FunctionDeclaration(Identifier("f"), { Identifier("p") },
                    IfConstruct(Literal(true),
                        Identifier("p"),
                        FunctionCall(Identifier("f"), { Identifier("p")})))
            },
            FunctionCall(Identifier("f"), { Literal(5) })
        )
    );
    BOOST_TEST(infer(funrecurse2) == Type(BasicType::INT));

    auto instantiate = Expression(
        LetConstruct(
            { FunctionDeclaration(Identifier("f"), { Identifier("p") }, Identifier("p")),
              VariableDeclaration(Identifier("x"), FunctionCall(Identifier("f"), { Literal(5) })),
              VariableDeclaration(Identifier("y"), FunctionCall(Identifier("f"), { Literal(.5) }))},
              Identifier("y")
        )
    );
    BOOST_TEST(infer(instantiate) == Type(BasicType::REAL));

    auto letx5int = Expression(
        LetConstruct(
            { VariableDeclaration(Identifier("x"), BasicType::INT, Literal(5)) },
            Identifier("x")
        )
    );
    BOOST_TEST(infer(letx5int) == Type(BasicType::INT));

    auto letx5fail = Expression(
        LetConstruct(
            { VariableDeclaration(Identifier("x"), BasicType::REAL, Literal(5)) },
            Identifier("x")
        )
    );
    BOOST_CHECK_THROW(infer(letx5fail), std::runtime_error);
}
