#include "lib.hpp"
#include "colliders/greedyCollider.hpp"
#include <iostream>
#include <vector>

// Collider class method definitions
GreedyCollider::Node::Node() : bbox(), left(nullptr), right(nullptr) {};

GreedyCollider::Node::Node(BBox bbox, NodeP &&child1, NodeP &&child2)
    : bbox(bbox), left((std::move(child1))), right(std::move(child2)) {};

GreedyCollider::Node::Node(BBox bbox) : bbox(bbox), left(nullptr), right(nullptr) {};

bool GreedyCollider::Node::isLeaf() { return !(left || right); }

GreedyCollider::Tree::Tree() : root{nullptr} {};

GreedyCollider::Tree::Tree(const std::vector<BBox> &objects) {
  GreedyCollider::Tree::build(objects);
}

void GreedyCollider::Tree::build(const std::vector<BBox> &objects) {
  for (auto bbox : objects) {
    insert(bbox);
  }
}

void GreedyCollider::Tree::insert(const BBox &bbox) {
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

GreedyCollider::Tree::Direction
GreedyCollider::Tree::minimalBoundingBox(const std::unique_ptr<Node> &node,
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

GreedyCollider::NodeP GreedyCollider::Tree::createNode(NodeP &child, const BBox &bbox) {
  auto bbox_node = std::make_unique<Node>(bbox);
  auto new_node =
      std::make_unique<Node>(BBox::boundingBoxUnion(bbox, child->bbox),
                             std::move(child), std::move(bbox_node));
  return new_node;
}

void GreedyCollider::Tree::recursiveInsert(NodeP &node, const NodeP &parent,
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

void GreedyCollider::Tree::recursiveCollisionDetection(
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

void GreedyCollider::Tree::recursiveCollisionDetectionMain(
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

void GreedyCollider::Tree::postOrderFlatten(const NodeP &node,
                                      std::vector<BBox> &vec) {
  if (!node) {
    return;
  }

  postOrderFlatten(node->left, vec);
  postOrderFlatten(node->right, vec);

  vec.push_back(node->bbox);
}

GreedyCollider::GreedyCollider(std::vector<BBox> &objects) : tree(objects) {};

std::vector<BBox> GreedyCollider::flatten() {
  std::vector<BBox> vec;
  tree.postOrderFlatten(tree.root, vec);
  return vec;
};

std::vector<std::pair<BBox, BBox>> GreedyCollider::collisionDetection() {
  std::vector<std::pair<BBox, BBox>> vec;
  if (!tree.root || tree.root->isLeaf())
    return vec;

  tree.count = 0;
  tree.recursiveCollisionDetectionMain(tree.root, vec);
  std::cout << "count is = " << tree.count << std::endl;

  return vec;
}
