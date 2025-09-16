#include <iterator>
#define BOOST_TEST_MODULE collision_detection_test
#include "lib.hpp"
#include <boost/test/unit_test.hpp>
#include <iostream>
#include <utility>
#include <vector>

std::ostream &operator<<(std::ostream &stream,
                         const std::pair<Obj, Obj> &other) {
  stream << '{' << other.first << other.second << '}';
  return stream;
}

BOOST_AUTO_TEST_SUITE(collision_pair_detection)

void checkCollision(std::vector<std::pair<Obj, Obj>> &result,
                    std::vector<std::pair<Obj, Obj>> &expected) {
  BOOST_CHECK_EQUAL(result.size(), expected.size());

  for (size_t i = 0; i < result.size(); i++) {
    BOOST_CHECK_EQUAL(result[i], expected[i]);
  }
}

BOOST_AUTO_TEST_CASE(root) {
  std::vector<Obj> objs{{{3, 4}, 2, 1}};

  Collider coll(objs);
  auto result = coll.collisionDetection();

  std::vector<std::pair<Obj, Obj>> expected{};

  checkCollision(result, expected);
}

BOOST_AUTO_TEST_CASE(two_objects_colliding) {
  std::vector<Obj> objs = {{{2, 3}, 3, 2}, {{0, 2}, 3, 4}};

  Collider coll(objs);
  auto result = coll.collisionDetection();

  std::vector<std::pair<Obj, Obj>> expected = {
      {{{2, 3}, 3, 2}, {{0, 2}, 3, 4}}
  };

  checkCollision(result, expected);
}

BOOST_AUTO_TEST_CASE(three_objects_colliding) {
  std::vector<Obj> objs = {
    {{0, 0}, 10, 10}, 
    {{2, 2}, 2, 2},
    {{20, 20}, 5, 5}
  };

  Collider coll(objs);
  auto result = coll.collisionDetection();

  std::vector<std::pair<Obj, Obj>> expected = {
    { {{0, 0}, 10, 10}, {{2, 2}, 2, 2} }
  };

  checkCollision(result, expected);
}

BOOST_AUTO_TEST_CASE(four_objects_colliding) {
  std::vector<Obj> objs = {
    {{0, 0}, 4, 4}, // A
    {{2, 2}, 4, 4}, // B
    {{5, 5}, 4, 4}, // C
    {{9, 9}, 2, 2}  // D
  };

  /*
   *        1
   *       / \
   *      2   3
   *     /\   /\
   *    A  B C  D
   */

  Collider coll(objs);
  auto result = coll.collisionDetection();

  std::vector<std::pair<Obj, Obj>> expected = {
    { {{2, 2}, 4, 4 }, {{5, 5}, 4, 4} },
    { {{0, 0}, 4, 4}, {{2, 2}, 4, 4} }
  };

  checkCollision(result, expected);
}

BOOST_AUTO_TEST_CASE(four_objects_irregular_sizes_colliding) {
  std::vector<Obj> objs = {
    {{0, 0}, 6, 5}, // A
    {{5, 4}, 3, 2}, // B
    {{2, 6}, 4, 5}, // C
    {{1, 4}, 2, 3}, // D
  };

  Collider coll(objs);
  std::vector<std::pair<Obj, Obj>> result = coll.collisionDetection();

  /*
   *        1
   *       / \
   *      2   C
   *     / \
   *    3   B
   *   / \
   *  A   D
   */

  std::vector<std::pair<Obj, Obj>> expected = {
    { {{2, 6}, 4, 5}, {{1, 4}, 2, 3} },
    { {{5, 4}, 3, 2}, {{0, 0}, 6, 5} },
    { {{0, 0}, 6, 5}, {{1, 4}, 2, 3} },
  };

  checkCollision(result, expected);
}

BOOST_AUTO_TEST_SUITE_END()
