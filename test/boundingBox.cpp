#define BOOST_TEST_MODULE bounding_box_test
#include <boost/test/unit_test.hpp>
#include "lib.hpp"

BOOST_AUTO_TEST_SUITE(union_test)

BOOST_AUTO_TEST_CASE(bounding_box_overlap) {
    BBox o1(Vec2(0, 0), 10, 10, 1);
    BBox o2(Vec2(5, 5), 10, 10, 2);

    BBox result = BBox::boundingBoxUnion(o1, o2);
    BBox expected {{0, 0}, 15, 15, 0};

    BOOST_CHECK_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(bounding_box_disjoint) {
    BBox o1(Vec2(0, 0), 10, 10, 1);
    BBox o2(Vec2(20, 20), 5, 5, 2);

    BBox result = BBox::boundingBoxUnion(o1, o2);
    BBox expected {{0, 0}, 25, 25, 0};

    BOOST_CHECK_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(bounding_box_area_overlap) {
    BBox o1(Vec2(0, 0), 10, 10, 1);
    BBox o2(Vec2(5, 5), 10, 10, 1);

    BOOST_CHECK_EQUAL(BBox::boundingBoxUnion(o1, o2).area(), 225);
}

BOOST_AUTO_TEST_CASE(bounding_box_area_disjoint) {
    BBox o1(Vec2(0, 0), 10, 10, 1);
    BBox o2(Vec2(20, 20), 5, 5, 2);

    BOOST_CHECK_EQUAL(BBox::boundingBoxUnion(o1, o2).area(), 625);
}

BOOST_AUTO_TEST_SUITE_END()
