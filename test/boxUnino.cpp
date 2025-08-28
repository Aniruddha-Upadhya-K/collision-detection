#define BOOST_TEST_MODULE example
#include <boost/test/unit_test.hpp>
#include "lib.hpp"

BOOST_AUTO_TEST_SUITE(union_test)

BOOST_AUTO_TEST_CASE(boxUnion_basic_overlap) {
    Obj o1(Pos(0, 0), 10, 10);
    Obj o2(Pos(5, 5), 10, 10);

    Obj result = boxUnion(o1, o2);

    BOOST_CHECK_EQUAL(result.p.x, 0);
    BOOST_CHECK_EQUAL(result.p.y, 0);
    BOOST_CHECK_EQUAL(result.w, 15);
    BOOST_CHECK_EQUAL(result.h, 15);
}

BOOST_AUTO_TEST_CASE(boxUnion_disjoint) {
    Obj o1(Pos(0, 0), 10, 10);
    Obj o2(Pos(20, 20), 5, 5);

    Obj result = boxUnion(o1, o2);

    BOOST_CHECK_EQUAL(result.p.x, 0);
    BOOST_CHECK_EQUAL(result.p.y, 0);
    BOOST_CHECK_EQUAL(result.w, 25);
    BOOST_CHECK_EQUAL(result.h, 25);
}

BOOST_AUTO_TEST_SUITE_END()
