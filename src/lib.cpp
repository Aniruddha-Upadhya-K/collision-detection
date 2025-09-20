#include "lib.hpp"
#include <algorithm>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

// Vec2 class methods definition
Vec2::Vec2(int x, int y) : val({x, y}) {};

bool Vec2::operator==(const Vec2 &other) const {
  return (this->val == other.val);
}

bool Vec2::operator!=(const Vec2 &other) const { return !(*this == other); }

Vec2 Vec2::operator+(const Vec2 &other) const {
  return {this->val[0] + other[0], this->val[1] + other[1]};
}

Vec2 Vec2::operator-(const Vec2 &other) const {
  return {this->val[0] - other.val[0], this->val[1] - other.val[1]};
}

int Vec2::operator[](std::size_t idx) const {
  if (idx > 1 || idx < 0)
    throw "Vec2: accessing out of bound index";
  return this->val[idx];
}

std::ostream &operator<<(std::ostream &stream, const Vec2 &vec) {
  stream << '(' << vec[0] << ", " << vec[1] << ')';
  return stream;
}

Vec2 Vec2::min(const Vec2 &vec1, const Vec2 &vec2) {
  return {std::min(vec1[0], vec2[0]), std::min(vec1[1], vec2[1])};
}

Vec2 Vec2::max(const Vec2 &vec1, const Vec2 &vec2) {
  return {std::max(vec1[0], vec2[0]), std::max(vec1[1], vec2[1])};
}

// BBox class methods definitions
BBox::BBox(const Vec2 &pos, const int width, const int height, int id)
    : _min_pt(pos), _max_pt(pos + Vec2{width, height}), id(id) {};

BBox::BBox(const Vec2 &min_pt, const Vec2 &max_pt)
    : _min_pt(min_pt), _max_pt(max_pt), id() {};

double BBox::area() const {
  Vec2 dist = _max_pt - _min_pt;
  return dist[0] * dist[1];
}

const BBox BBox::boundingBoxUnion(const BBox &bbox1, const BBox &bbox2) {
  return {Vec2::min(bbox1._min_pt, bbox2._min_pt),
          Vec2::max(bbox1._max_pt, bbox2._max_pt)};
}

bool BBox::isBoundingBoxIntersection(const BBox &bbox1, const BBox &bbox2) {
  bool intersection_x = bbox1._min_pt[0] <= bbox2._max_pt[0] &&
                        bbox1._max_pt[0] >= bbox2._min_pt[0];

  bool intersection_y = bbox1._min_pt[1] <= bbox2._max_pt[1] &&
                        bbox1._max_pt[1] >= bbox2._min_pt[1];

  return intersection_x && intersection_y;
}

bool BBox::operator==(const BBox &other) const {
  return (this->id == other.id && this->_max_pt == other._max_pt &&
          this->_min_pt == other._min_pt);
}

bool BBox::operator!=(const BBox &other) const { return !(*this == other); }

std::ostream &operator<<(std::ostream &stream, const BBox &bbox) {
  stream << "Object " << bbox.id << std::endl;
  stream << "min = " << bbox._min_pt << std::endl;
  stream << "max = " << bbox._max_pt << std::endl;
  return stream;
}

// Collider class method definitions
Collider::Node::Node() : bbox(), left(nullptr), right(nullptr) {};

Collider::Node::Node(BBox bbox, NodeP &&child1, NodeP &&child2)
    : bbox(bbox), left((std::move(child1))), right(std::move(child2)) {};

Collider::Node::Node(BBox bbox) : bbox(bbox), left(nullptr), right(nullptr) {};

bool Collider::Node::isLeaf() { return !(left || right); }

Collider::Tree::Tree() : root{nullptr} {};

Collider::Tree::Tree(const std::vector<BBox> &objects) {
  Collider::Tree::build(objects);
}

void Collider::Tree::build(const std::vector<BBox> &objects) {
  for (auto bbox : objects) {
    insert(bbox);
  }
}

void Collider::Tree::insert(const BBox &bbox) {
  if (!root) {
    root = std::make_unique<Node>(bbox);
    return;
  }

  if (root->isLeaf()) {
    root = createNode(root, bbox);
    return;
  }

  recursiveInsert(root, nullptr, bbox);
}

Collider::Tree::Direction
Collider::Tree::minimalBoundingBox(const std::unique_ptr<Node> &node,
                                   const BBox &bbox) {
  double l_area{}, r_area{}, s_area{};
  Direction dir{};

  s_area = node->bbox.area() + bbox.area();
  l_area = BBox::boundingBoxUnion(bbox, node->left->bbox).area() +
           node->right->bbox.area();
  r_area = BBox::boundingBoxUnion(bbox, node->right->bbox).area() +
           node->left->bbox.area();

  if (r_area < l_area) {
    if (r_area < s_area)
      dir = RIGHT;
    else
      dir = SIBLING;

  } else {
    if (l_area < s_area)
      dir = LEFT;
    else
      dir = SIBLING;
  }

  return dir;
}

Collider::NodeP Collider::Tree::createNode(NodeP &child, const BBox &bbox) {
  auto bbox_node = std::make_unique<Node>(bbox);
  auto new_node =
      std::make_unique<Node>(BBox::boundingBoxUnion(bbox, child->bbox),
                             std::move(child), std::move(bbox_node));
  return new_node;
}

void Collider::Tree::recursiveInsert(NodeP &node, const NodeP &parent,
                                     const BBox &bbox) {
  switch (minimalBoundingBox(node, bbox)) {
  case SIBLING: {
    auto new_node = createNode(node, bbox);

    if (!parent) {
      root = std::move(new_node);
      return;
    }

    if (parent->left == node)
      parent->left = std::move(new_node);
    else
      parent->right = std::move(new_node);
    return;
  }
  case LEFT: {
    if (node->left->isLeaf()) {
      node->left = createNode(node->left, bbox);
      break;
    }

    recursiveInsert(node->left, node, bbox);
    break;
  }
  case RIGHT: {
    if (node->right->isLeaf()) {
      node->right = createNode(node->right, bbox);
      break;
    }

    recursiveInsert(node->right, node, bbox);
    break;
  }
  }

  // update dimension of parent objects
  if (BBox::boundingBoxUnion(node->left->bbox, node->right->bbox).area() !=
      node->bbox.area()) {
    node->bbox = BBox::boundingBoxUnion(node->left->bbox, node->right->bbox);
  }
}

void Collider::Tree::recursiveCollisionDetection(
    const NodeP &child1, const NodeP &child2,
    std::vector<std::pair<BBox, BBox>> &vec) {
  if (!child1 || !child2)
    return;

  count++;

  if (BBox::isBoundingBoxIntersection(child1->bbox, child2->bbox)) {
    if (child1->isLeaf() && child2->isLeaf()) {
      vec.push_back({child1->bbox, child2->bbox});
      return;
    }

    if (!child1->isLeaf() && !child2->isLeaf()) {
      recursiveCollisionDetection(child1->left, child2->left, vec);
      recursiveCollisionDetection(child1->left, child2->right, vec);
      recursiveCollisionDetection(child1->right, child2->left, vec);
      recursiveCollisionDetection(child1->right, child2->right, vec);
      return;
    }

    if (child1->isLeaf()) {
      recursiveCollisionDetection(child1, child2->left, vec);
      recursiveCollisionDetection(child1, child2->right, vec);
      return;
    }

    if (child2->isLeaf()) {
      recursiveCollisionDetection(child2, child1->left, vec);
      recursiveCollisionDetection(child2, child1->right, vec);
    }
  }
}

void Collider::Tree::recursiveCollisionDetectionMain(
    const NodeP &node, std::vector<std::pair<BBox, BBox>> &vec) {
  if (!node)
    return;
  if (node->isLeaf())
    return;

  recursiveCollisionDetection(node->left, node->right, vec);

  if (!node->left->isLeaf()) {
    recursiveCollisionDetectionMain(node->left, vec);
  }
  if (!node->right->isLeaf()) {
    recursiveCollisionDetectionMain(node->right, vec);
  }
}

void Collider::Tree::postOrderFlatten(const NodeP &node,
                                      std::vector<BBox> &vec) {
  if (!node) {
    return;
  }

  postOrderFlatten(node->left, vec);
  postOrderFlatten(node->right, vec);

  vec.push_back(node->bbox);
}

Collider::Collider(std::vector<BBox> &objects) : tree(objects) {};

std::vector<BBox> Collider::flatten() {
  std::vector<BBox> vec;
  tree.postOrderFlatten(tree.root, vec);
  return vec;
};

std::vector<std::pair<BBox, BBox>> Collider::collisionDetection() {
  std::vector<std::pair<BBox, BBox>> vec;
  if (!tree.root || tree.root->isLeaf())
    return vec;

  tree.count = 0;
  tree.recursiveCollisionDetectionMain(tree.root, vec);
  std::cout << "count is = " << tree.count << std::endl;

  return vec;
}
