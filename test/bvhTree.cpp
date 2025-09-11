#include <cassert>
#define BOOST_TEST_MODULE bvh_tree
#include <boost/test/unit_test.hpp>
#include <vector>
#include "lib.hpp"

BOOST_AUTO_TEST_SUITE(bvh_tree_test)

void check_obj_vec(const size_t num_objs,const std::vector<Obj> &result, const std::vector<Obj> &expected) {
  size_t size = 2 * num_objs - 1; // Given the number of leaf node, there will always be 2n-1 nodes in a binary tree

  assert(expected.size() == size);
  BOOST_CHECK_EQUAL(result.size(), size);

  for (size_t i = 0; i < size; i++) {
    BOOST_CHECK_EQUAL(result[i], expected[i]);
  }
}

BOOST_AUTO_TEST_CASE(root) { 
  Obj o1 {{3, 4}, 2, 1}; 
  std::vector<Obj> objs { o1 };

  Collider coll(objs);
  std::vector<Obj> result = coll.debug();

  std::vector<Obj> expected = {
    {{3, 4}, 2, 1}
  };

  check_obj_vec(objs.size(), result, expected);
}

BOOST_AUTO_TEST_CASE(two_objects) {
  std::vector<Obj> objs = {
    {{3, 2}, 2, 3},
    {{1, 1}, 1, 1}
  };

  Collider coll(objs);
  std::vector<Obj> result = coll.debug();

  std::vector<Obj> expected = {
    {{3, 2}, 2, 3},
    {{1, 1}, 1, 1},
    {{1, 1}, 4, 4}
  };
  
  check_obj_vec(objs.size(), result, expected);
}

BOOST_AUTO_TEST_CASE(three_objects_nested_and_disjoint) {
  std::vector<Obj> objs = {
    {{0, 0}, 10, 10}, 
    {{2, 2}, 2, 2},
    {{20, 20}, 5, 5}
  };

  Collider coll(objs);
  std::vector<Obj> result = coll.debug();

  std::vector<Obj> expected = {
    {{0, 0}, 10, 10},
    {{2, 2}, 2, 2},
    {{0, 0}, 10, 10},
    {{20, 20}, 5, 5},
    {{0, 0}, 25, 25}
  };

  check_obj_vec(objs.size(), result, expected);
}

BOOST_AUTO_TEST_CASE(four_objects_chain_overlap) {
  std::vector<Obj> objs = {
    {{0, 0}, 4, 4},
    {{2, 2}, 4, 4},
    {{5, 5}, 4, 4},
    {{9, 9}, 2, 2}
  };

  Collider coll(objs);
  std::vector<Obj> result = coll.debug();

  std::vector<Obj> expected = {
    {{0, 0}, 4, 4},
    {{2, 2}, 4, 4},
    {{0, 0}, 6, 6},
    {{5, 5}, 4, 4},
    {{9, 9}, 2, 2},
    {{5, 5}, 6, 6},
    {{0, 0}, 11, 11}
  };

  check_obj_vec(objs.size(), result, expected);
}

BOOST_AUTO_TEST_CASE(five_objects_varied) {
  std::vector<Obj> objs = {
    {{0, 0}, 5, 5},
    {{6, 4}, 2, 2},
    {{3, 10}, 5, 5},
    {{2, 6}, 2, 2},
  };

  Collider coll(objs);
  std::vector<Obj> result = coll.debug();

  std::vector<Obj> expected = {
    {{0, 0}, 5, 5},
    {{2, 6}, 2, 2},
    {{0, 0}, 5, 8},
    {{6, 4}, 2, 2},
    {{0, 0}, 8, 8},
    {{3, 10}, 5, 5},
    {{0, 0}, 8, 15},
  };

  check_obj_vec(objs.size(), result, expected);
}

BOOST_AUTO_TEST_SUITE_END()
