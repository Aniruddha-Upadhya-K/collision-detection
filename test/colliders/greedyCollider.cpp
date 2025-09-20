#define BOOST_TEST_MODULE collision_detection_test
#include <boost/test/unit_test.hpp>
#include <iostream>
#include <utility>
#include <vector>
#include "lib.hpp"
#include "colliders/greedyCollider.hpp"

std::ostream &operator<<(std::ostream &stream,
    const std::pair<BBox, BBox> &other) {
  stream << '{' << other.first << other.second << '}';
  return stream;
}

BOOST_AUTO_TEST_SUITE(greedy_collision_pair_detection)

void checkCollision(std::vector<std::pair<BBox, BBox>> &result,
                    std::vector<std::pair<BBox, BBox>> &expected) {
  BOOST_CHECK_EQUAL(result.size(), expected.size());

  for (size_t i = 0; i < result.size(); i++) {
    BOOST_CHECK_EQUAL(result[i], expected[i]);
  }
}

BOOST_AUTO_TEST_CASE(greedy_root) {
  std::vector<BBox> objs{{{3, 4}, 2, 1, 1}};

  GreedyCollider coll(objs);
  auto result = coll.collisionDetection();

  std::vector<std::pair<BBox, BBox>> expected{};

  checkCollision(result, expected);
}

BOOST_AUTO_TEST_CASE(greedy_two_objects_colliding) {
  std::vector<BBox> objs = {{{2, 3}, 3, 2, 1}, {{0, 2}, 3, 4, 2}};

  GreedyCollider coll(objs);
  auto result = coll.collisionDetection();

  std::vector<std::pair<BBox, BBox>> expected = {
      {{{2, 3}, 3, 2, 1}, {{0, 2}, 3, 4, 2}}
  };

  checkCollision(result, expected);
}

BOOST_AUTO_TEST_CASE(greedy_three_objects_colliding) {
  std::vector<BBox> objs = {
    {{0, 0}, 10, 10, 1}, 
    {{2, 2}, 2, 2, 2},
    {{20, 20}, 5, 5, 3}
  };

  GreedyCollider coll(objs);
  auto result = coll.collisionDetection();

  std::vector<std::pair<BBox, BBox>> expected = {
    { {{0, 0}, 10, 10, 1}, {{2, 2}, 2, 2, 2} }
  };

  checkCollision(result, expected);
}

BOOST_AUTO_TEST_CASE(greedy_four_objects_colliding) {
  std::vector<BBox> objs = {
    {{0, 0}, 4, 4, 1}, // A
    {{2, 2}, 4, 4, 2}, // B
    {{5, 5}, 4, 4, 3}, // C
    {{9, 9}, 2, 2, 4}  // D
  };

  /*
   *        1
   *       / \
   *      2   3
   *     /\   /\
   *    A  B C  D
   */

  GreedyCollider coll(objs);
  auto result = coll.collisionDetection();

  std::vector<std::pair<BBox, BBox>> expected = {
    { {{2, 2}, 4, 4, 2}, {{5, 5}, 4, 4, 3} },
    { {{0, 0}, 4, 4, 1}, {{2, 2}, 4, 4, 2} },
    { {{5, 5}, 4, 4, 3}, {{9, 9}, 2, 2, 4} }
  };

  checkCollision(result, expected);
}

BOOST_AUTO_TEST_CASE(greedy_four_objects_irregular_sizes_colliding) {
  std::vector<BBox> objs = {
    {{0, 0}, 6, 5, 1}, // A
    {{5, 4}, 3, 2, 2}, // B
    {{2, 6}, 4, 5, 3}, // C
    {{1, 4}, 2, 3, 4}, // D
  };

  GreedyCollider coll(objs);
  std::vector<std::pair<BBox, BBox>> result = coll.collisionDetection();

  /*
   *        1
   *       / \
   *      2   C
   *     / \
   *    3   B
   *   / \
   *  A   D
   */

  std::vector<std::pair<BBox, BBox>> expected = {
    { {{2, 6}, 4, 5, 3}, {{1, 4}, 2, 3, 4} },
    { {{2, 6}, 4, 5, 3}, {{5, 4}, 3, 2, 2} },
    { {{5, 4}, 3, 2, 2}, {{0, 0}, 6, 5, 1} },
    { {{0, 0}, 6, 5, 1}, {{1, 4}, 2, 3, 4} },
  };

  checkCollision(result, expected);
}

BOOST_AUTO_TEST_SUITE_END()
