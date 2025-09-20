#ifndef GREEDY_COLLIDER
#define GREEDY_COLLIDER

#include <vector>
#include "lib.hpp"

class GreedyCollider {
public:
  GreedyCollider();
  explicit GreedyCollider(std::vector<BBox> &objects);

  std::vector<BBox> flatten();

  std::vector<std::pair<BBox, BBox>> collisionDetection();

private:
  struct Node {
    using NodeP = std::unique_ptr<Node>;

    BBox bbox;
    NodeP left, right;

    Node();
    Node(BBox bbox, NodeP &&child1, NodeP &&child2);

    explicit Node(BBox bbox);

    bool isLeaf();
  };
  typedef std::unique_ptr<Node> NodeP;

  class Tree {
  public:
    int count;
    std::unique_ptr<Node> root;

    Tree();
    explicit Tree(const std::vector<BBox> &objects);

    void build(const std::vector<BBox> &objects);

    void insert(const BBox &bbox);

    void recursiveCollisionDetectionMain(const NodeP &node,
                                         std::vector<std::pair<BBox, BBox>> &vec);

    void recursiveCollisionDetection(const NodeP &child1, const NodeP &child2,
                                     std::vector<std::pair<BBox, BBox>> &vec);

    void postOrderFlatten(const NodeP &node, std::vector<BBox> &vec);

  private:
    enum Direction { LEFT, RIGHT, SIBLING };

    Direction minimalBoundingBox(const std::unique_ptr<Node> &node,
                                 const BBox &bbox);

    NodeP createNode(NodeP &child, const BBox &bbox);

    void recursiveInsert(NodeP &node, const NodeP &parent, const BBox &bbox);
  };

  Tree tree;
};

#endif // !GREEDY_COLLIDER
