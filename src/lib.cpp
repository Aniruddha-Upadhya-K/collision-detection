#include "lib.hpp"
#include <algorithm>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

// Pos class methods definition
Pos::Pos() : _x(0), _y(0) {};

Pos::Pos(int x, int y) : _x(x), _y(y) {};

void Pos::update(int delta_x = 0, int delta_y = 0) {
  _x += delta_x;
  _y += delta_y;
}

int Pos::getX() const { return _x; }

void Pos::setX(int x) { _x = x; }

int Pos::getY() const { return _y; }

void Pos::setY(int y) { _y = y; }

const std::array<int, 2> Pos::xy() const { return {_x, _y}; }

bool Pos::operator==(const Pos &other) const {
  return (_x == other._x && _y == other._y);
}

bool Pos::operator!=(const Pos &other) const { return !(*this == other); }

// Obj class methods definitions
Obj::Obj() : _pos(), _w(0), _h(0) {};

Obj::Obj(const Pos &pos, const int width, const int height)
    : _pos(pos), _w(width), _h(height) {};

const std::array<int, 2> Obj::pos() const { return _pos.xy(); }

void Obj::pos(const Pos &pos) { _pos = pos; }

int Obj::getHeight() const { return _h; }

void Obj::setHeight(int height) { _h = height; }

int Obj::getWidth() const { return _w; }

void Obj::setWidth(int width) { _w = width; }

int Obj::area() const { return _w * _h; }

const Obj Obj::boundingBoxUnion(const Obj &obj1, const Obj &obj2) {
  auto p1 = obj1.pos();
  auto p2 = obj2.pos();

  Pos p{std::min(p1[0], p2[0]), std::min(p1[1], p2[1])};
  int w = std::max(p1[0] + obj1._w, p2[0] + obj2._w) - p.getX();
  int h = std::max(p1[1] + obj1._h, p2[1] + obj2._h) - p.getY();
  Obj o{p, w, h};
  return o;
}

int Obj::boundingBoxUnionArea(const Obj &obj1, const Obj &obj2) {
  const std::array<int, 2> &p1 = obj1.pos();
  const std::array<int, 2> &p2 = obj2.pos();

  Pos p{std::min(p1[0], p2[0]), std::min(p1[1], p2[1])};
  int w = std::max(p1[0] + obj1._w, p2[0] + obj2._w) - p.getX();
  int h = std::max(p1[1] + obj1._h, p2[1] + obj2._h) - p.getY();

  return h * w;
}

bool Obj::isBoundingBoxIntersection(const Obj &obj1, const Obj &obj2) {
  int o1_left = obj1._pos.getX();
  int o1_right = obj1._pos.getX() + obj1._w;
  int o1_top = obj1._pos.getY();
  int o1_bottom = obj1._pos.getY() + obj1._h;

  int o2_left = obj2._pos.getX();
  int o2_right = obj2._pos.getX() + obj2._w;
  int o2_top = obj2._pos.getY();
  int o2_bottom = obj2._pos.getY() + obj2._h;

  bool intersection_x = o1_left < o2_right && o1_right > o2_left;
  bool intersection_y = o1_top < o2_bottom && o1_bottom > o2_top;

  return intersection_x && intersection_y;
}

bool Obj::operator==(const Obj &other) const {
  return (_pos == other._pos && _h == other._h && _w == other._w);
}

bool Obj::operator!=(const Obj &other) const { return !(*this == other); }

std::ostream &operator<<(std::ostream &stream, const Obj &obj) {
  stream << "Object:\n\t(x, y) = (" << obj.pos()[0] << ", " << obj.pos()[1]
         << ")" << std::endl;
  stream << "\twidth = " << obj.getWidth() << std::endl;
  stream << "\theight = " << obj.getHeight() << std::endl;
  return stream;
}

Collider::Node::Node() : obj(), left(nullptr), right(nullptr) {};

Collider::Node::Node(Obj obj, NodeP &&child1, NodeP &&child2)
    : obj(obj), left((std::move(child1))), right(std::move(child2)) {};

Collider::Node::Node(Obj obj) : obj(obj), left(nullptr), right(nullptr) {};

bool Collider::Node::isLeaf() { return !(left || right); }

Collider::Tree::Tree() : root{nullptr} {};

Collider::Tree::Tree(const std::vector<Obj> &objects) {
  Collider::Tree::build(objects);
}

void Collider::Tree::build(const std::vector<Obj> &objects) {
  for (auto obj : objects) {
    insert(obj);
  }
}

void Collider::Tree::insert(const Obj &obj) {
  if (!root) {
    root = std::make_unique<Node>(obj);
    return;
  }

  if (root->isLeaf()) {
    root = createNode(root, obj);
    return;
  }

  recursiveInsert(root, nullptr, obj);
}

Collider::Tree::Direction
Collider::Tree::minimalBoundingBox(const std::unique_ptr<Node> &node,
                                   const Obj &obj) {
  int l_area{}, r_area{}, s_area{};
  Direction dir{};

  s_area = node->obj.area() + obj.area();
  l_area =
      Obj::boundingBoxUnionArea(obj, node->left->obj) + node->right->obj.area();
  r_area =
      Obj::boundingBoxUnionArea(obj, node->right->obj) + node->left->obj.area();

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

Collider::NodeP Collider::Tree::createNode(NodeP &child, const Obj &obj) {
  auto obj_node = std::make_unique<Node>(obj);
  auto new_node = std::make_unique<Node>(Obj::boundingBoxUnion(obj, child->obj),
                                         std::move(child), std::move(obj_node));
  return new_node;
}

void Collider::Tree::recursiveInsert(NodeP &node, const NodeP &parent,
                                     const Obj &obj) {
  switch (minimalBoundingBox(node, obj)) {
  case SIBLING: {
    auto new_node = createNode(node, obj);

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
      node->left = createNode(node->left, obj);
      break;
    }

    recursiveInsert(node->left, node, obj);
    break;
  }
  case RIGHT: {
    if (node->right->isLeaf()) {
      node->right = createNode(node->right, obj);
      break;
    }

    recursiveInsert(node->right, node, obj);
    break;
  }
  }

  // update dimension of parent objects
  if (Obj::boundingBoxUnionArea(node->left->obj, node->right->obj) !=
      node->obj.area()) {
    node->obj = Obj::boundingBoxUnion(node->left->obj, node->right->obj);
  }
}

void Collider::Tree::recursiveCollisionDetection(
    const NodeP &child1, const NodeP &child2,
    std::vector<std::pair<Obj, Obj>> &vec) {
  if (!child1 || !child2)
    return;

  count++;

  if (Obj::isBoundingBoxIntersection(child1->obj, child2->obj)) {
    if (child1->isLeaf() && child2->isLeaf()) {
      vec.push_back({child1->obj, child2->obj});
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
    const NodeP &node, std::vector<std::pair<Obj, Obj>> &vec) {
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
                                      std::vector<Obj> &vec) {
  if (!node) {
    return;
  }

  postOrderFlatten(node->left, vec);
  postOrderFlatten(node->right, vec);

  vec.push_back(node->obj);
}

Collider::Collider(std::vector<Obj> &objects) : tree(objects) {};

std::vector<Obj> Collider::flatten() {
  std::vector<Obj> vec;
  tree.postOrderFlatten(tree.root, vec);
  return vec;
};

std::vector<std::pair<Obj, Obj>> Collider::collisionDetection() {
  std::vector<std::pair<Obj, Obj>> vec;
  if (!tree.root || tree.root->isLeaf())
    return vec;

  tree.count = 0;
  tree.recursiveCollisionDetectionMain(tree.root, vec);
  std::cout<<"count is = "<<tree.count<<std::endl;

  return vec;
}
