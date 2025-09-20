#define BOOST_TEST_MODULE bvh_tree_test
#include <boost/test/unit_test.hpp>
#include <cassert>
#include <vector>
#include "lib.hpp"
#include "colliders/greedyCollider.hpp"

BOOST_AUTO_TEST_SUITE(greedy_bvh_tree_test)

void checkBBoxVec(const size_t num_objs,const std::vector<BBox> &result, const std::vector<BBox> &expected) {
  size_t size = 2 * num_objs - 1; // Given the number of leaf node, there will always be 2n-1 nodes in a binary tree

  assert(expected.size() == size);
  BOOST_CHECK_EQUAL(result.size(), size);

  for (size_t i = 0; i < size; i++) {
    BOOST_CHECK_EQUAL(result[i], expected[i]);
  }
}

BOOST_AUTO_TEST_CASE(root) { 
  BBox o1 {{3, 4}, 2, 1, 1}; 
  std::vector<BBox> objs { o1 };

  GreedyCollider coll(objs);
  std::vector<BBox> result = coll.flatten();

  std::vector<BBox> expected = {
    {{3, 4}, 2, 1, 1}
  };

  checkBBoxVec(objs.size(), result, expected);
}

BOOST_AUTO_TEST_CASE(two_objects) {
  std::vector<BBox> objs = {
    {{3, 2}, 2, 3, 1},
    {{1, 1}, 1, 1, 2}
  };

  GreedyCollider coll(objs);
  std::vector<BBox> result = coll.flatten();

  std::vector<BBox> expected = {
    {{3, 2}, 2, 3, 1},
    {{1, 1}, 1, 1, 2},
    {{1, 1}, 4, 4, 0}
  };
  
  checkBBoxVec(objs.size(), result, expected);
}

BOOST_AUTO_TEST_CASE(three_objects_nested_and_disjoint) {
  std::vector<BBox> objs = {
    {{0, 0}, 10, 10, 1}, 
    {{2, 2}, 2, 2, 2},
    {{20, 20}, 5, 5, 3}
  };

  GreedyCollider coll(objs);
  std::vector<BBox> result = coll.flatten();

  std::vector<BBox> expected = {
    {{0, 0}, 10, 10, 1},
    {{2, 2}, 2, 2, 2},
    {{0, 0}, 10, 10, 0},
    {{20, 20}, 5, 5, 3},
    {{0, 0}, 25, 25, 0}
  };

  checkBBoxVec(objs.size(), result, expected);
}

BOOST_AUTO_TEST_CASE(four_objects_chain_overlap) {
  std::vector<BBox> objs = {
    {{0, 0}, 4, 4, 1},
    {{2, 2}, 4, 4, 2},
    {{5, 5}, 4, 4, 3},
    {{9, 9}, 2, 2, 4}
  };

  GreedyCollider coll(objs);
  std::vector<BBox> result = coll.flatten();

  std::vector<BBox> expected = {
    {{0, 0}, 4, 4, 1},
    {{2, 2}, 4, 4, 2},
    {{0, 0}, 6, 6, 0},
    {{5, 5}, 4, 4, 3},
    {{9, 9}, 2, 2, 4},
    {{5, 5}, 6, 6, 0},
    {{0, 0}, 11, 11, 0}
  };

  checkBBoxVec(objs.size(), result, expected);
}

BOOST_AUTO_TEST_CASE(four_objects_disjoint) {
  std::vector<BBox> objs = {
    {{0, 0}, 5, 5, 1},
    {{6, 4}, 2, 2, 2},
    {{3, 10}, 5, 5, 3},
    {{2, 6}, 2, 2, 4},
  };

  GreedyCollider coll(objs);
  std::vector<BBox> result = coll.flatten();

  std::vector<BBox> expected = {
    {{0, 0}, 5, 5, 1},
    {{2, 6}, 2, 2, 4},
    {{0, 0}, 5, 8, 0},
    {{6, 4}, 2, 2, 2},
    {{0, 0}, 8, 8, 0},
    {{3, 10}, 5, 5, 3},
    {{0, 0}, 8, 15, 0},
  };

  checkBBoxVec(objs.size(), result, expected);
}

BOOST_AUTO_TEST_SUITE_END()
