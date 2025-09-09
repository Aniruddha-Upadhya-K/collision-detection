#include "lib.hpp"
#include <algorithm>
#include <iostream>
#include <memory>

// Pos class methods definition
Pos::Pos() : _x(0), _y(0) {};

Pos::Pos(int x, int y) : _x(x), _y(y) {};

void Pos::update(int delta_x = 0, int delta_y = 0) {
  _x += delta_x;
  _y += delta_y;
}

int Pos::x() { return _x; }

void Pos::x(int x) { _x = x; }

int Pos::y() { return _y; }

void Pos::y(int y) { _y = y; }

const std::array<int, 2> Pos::xy() const { return {_x, _y}; }

// Obj class methods definitions
Obj::Obj() : _pos(), _h(0), _w(0) {};

Obj::Obj(const Pos &pos, const int height, const int width)
    : _pos(pos), _h(height), _w(width) {};

const std::array<int, 2> Obj::pos() const { return _pos.xy(); }

void Obj::pos(const Pos &pos) { _pos = pos; }

int Obj::height() { return _h; }

void Obj::height(int height) { _h = height; }

int Obj::width() { return _w; }

void Obj::width(int width) { _w = width; }

int Obj::area() const { return _w * _h; }

const Obj Obj::boundingBoxUnion(const Obj &obj1, const Obj &obj2) {
  auto p1 = obj1.pos();
  auto p2 = obj2.pos();

  Pos p{std::min(p1[0], p2[0]), std::min(p1[1], p2[1])};
  int w = std::max(p1[0] + obj1._w, p2[0] + obj2._w) - p.x();
  int h = std::max(p1[1] + obj1._h, p2[1] + obj2._h) - p.y();
  Obj o{p, h, w};
  return o;
}

int Obj::boundingBoxUnionArea(const Obj &obj1, const Obj &obj2) {
  const std::array<int, 2> &p1 = obj1.pos();
  const std::array<int, 2> &p2 = obj2.pos();

  Pos p{std::min(p1[0], p2[0]), std::min(p1[1], p2[1])};
  int w = std::max(p1[0] + obj1._w, p2[0] + obj2._w) - p.x();
  int h = std::max(p1[1] + obj1._h, p2[1] + obj2._h) - p.y();

  return h * w;
}

Collider::Node::Node() : obj(), left(nullptr), right(nullptr) {};

Collider::Node::Node(Obj obj, NodeP &&child1, NodeP &&child2)
    : obj(obj), left((std::move(child1))), right(std::move(child2)) {};

Collider::Node::Node(Obj obj) : obj(obj), left(nullptr), right(nullptr) {};

bool Collider::Node::is_leaf() { return !(left || right); }

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

  if (root->is_leaf()) {
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
  l_area = Obj::boundingBoxUnionArea(obj, node->left->obj) + node->right->obj.area();
  r_area = Obj::boundingBoxUnionArea(obj, node->right->obj) + node->left->obj.area();

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
    if (node->left->is_leaf()) {
      node->left = createNode(node->left, obj);
      break;
    }

    recursiveInsert(node->left, node, obj);
    break;
  }
  case RIGHT: {
    if (node->right->is_leaf()) {
      node->right = createNode(node->right, obj);
      break;
    }

    recursiveInsert(node->right, node, obj);
    break;
  }
  }

  // update dimension of parent objects
  if (Obj::boundingBoxUnionArea(node->left->obj, node->right->obj) == node->obj.area()) {
    return;
  }

  node->obj = Obj::boundingBoxUnion(node->left->obj, node->right->obj);
}

void Collider::Tree::postOrderDebug(const NodeP &node) {
  if (!node) {
    return;
  }

  postOrderDebug(node->left);
  postOrderDebug(node->right);

  Obj &obj = node->obj;

  std::cout << "Object : \nposition = " << obj.pos()[0] << ", " << obj.pos()[1]
            << std::endl;
  std::cout << "width = " << obj.width() << std::endl;
  std::cout << "height = " << obj.height() << std::endl;
  std::cout << "\n\n\n";
}

Collider::Collider(std::vector<Obj> &objects) : tree(objects) {};
