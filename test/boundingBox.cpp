#define BOOST_TEST_MODULE bounding_box_test
#include <boost/test/unit_test.hpp>
#include "lib.hpp"

BOOST_AUTO_TEST_SUITE(union_test)

BOOST_AUTO_TEST_CASE(bounding_box_overlap) {
    Obj o1(Pos(0, 0), 10, 10);
    Obj o2(Pos(5, 5), 10, 10);

    Obj result = Obj::boundingBoxUnion(o1, o2);
    Obj expected {{0, 0}, 15, 15};

    BOOST_CHECK_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(bounding_box_disjoint) {
    Obj o1(Pos(0, 0), 10, 10);
    Obj o2(Pos(20, 20), 5, 5);

    Obj result = Obj::boundingBoxUnion(o1, o2);
    Obj expected {{0, 0}, 25, 25};

    BOOST_CHECK_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(bounding_box_area_overlap) {
    Obj o1(Pos(0, 0), 10, 10);
    Obj o2(Pos(5, 5), 10, 10);

    BOOST_CHECK_EQUAL(Obj::boundingBoxUnionArea(o1, o2), 225);
}

BOOST_AUTO_TEST_CASE(bounding_box_area_disjoint) {
    Obj o1(Pos(0, 0), 10, 10);
    Obj o2(Pos(20, 20), 5, 5);

    BOOST_CHECK_EQUAL(Obj::boundingBoxUnionArea(o1, o2), 625);
}

BOOST_AUTO_TEST_SUITE_END()
