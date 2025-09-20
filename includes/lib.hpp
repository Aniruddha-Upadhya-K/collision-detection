#ifndef LIB
#define LIB

#include <array>
#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

class Vec2 {
public:
  std::array<int, 2> val;

  Vec2() = default;

  Vec2(int x, int y);

  bool operator==(const Vec2 &other) const;
  bool operator!=(const Vec2 &other) const;

  Vec2 operator+(const Vec2 &other) const;
  Vec2 operator-(const Vec2 &other) const;
  int operator[](std::size_t idx) const;
  
  static Vec2 min(const Vec2 &vec1, const Vec2 &vec2);
  static Vec2 max(const Vec2 &vec1, const Vec2 &vec2) ;

private:
  friend std::ostream &operator<<(std::ostream &stream, const Vec2 &vec);
};

class BBox{
public:
  static const BBox boundingBoxUnion(const BBox &bbox1, const BBox &bbox2);

  static bool isBoundingBoxIntersection(const BBox &bbox1, const BBox &bbox2);

public:
  BBox() = default;
  BBox(const Vec2 &pos, const int width, const int height, int id);
  BBox(const Vec2 &min_pt, const Vec2 &max_pt);

  double area() const;

  bool operator==(const BBox &other) const;
  bool operator!=(const BBox &other) const;

private:
  Vec2 _min_pt, _max_pt;
  int id;

private:
  friend std::ostream &operator<<(std::ostream &stream, const BBox &bbox);
};


class Collider {
public:
  Collider();
  explicit Collider(std::vector<BBox> &objects);

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

#endif // !LIB
