#ifndef LIB
#define LIB

#include <array>
#include <cstddef>
#include <memory>
#include <vector>

class Pos {
public:
  Pos();

  Pos(int x, int y);

  void update(int delta_x, int delta_y);

  int x();
  void x(int x);

  int y();
  void y(int y);

  const std::array<int, 2> xy() const;

private:
  int _x, _y;
};

class Obj {
  // TODO: copy/move constructor needed
public:
  Obj();
  Obj(const Pos &pos, const int height, const int width);

  const std::array<int, 2> pos() const;
  void pos(const Pos &pos);

  int height();
  void height(int height);

  int width();
  void width(int width);

  int area() const;

  // make these two methods static
  static const Obj boundingBoxUnion(const Obj &obj1, const Obj &obj2);
  static int boundingBoxUnionArea(const Obj &obj1, const Obj &obj2);

private:
  Pos _pos;
  int _h, _w;
};

class Collider {
public:
  Collider();
  explicit Collider(std::vector<Obj> &objects);

  void debug() { tree.debug(); }

private:
  struct Node {
    using NodeP = std::unique_ptr<Node>;

    Obj obj;
    NodeP left, right;

    Node();
    Node(Obj obj, NodeP &&child1, NodeP &&child2);

    explicit Node(Obj obj);

    bool is_leaf();
  };
  typedef std::unique_ptr<Node> NodeP;

  class Tree {
  public:
    std::unique_ptr<Node> root;

    Tree();
    explicit Tree(const std::vector<Obj> &objects);

    void build(const std::vector<Obj> &objects);

    void insert(const Obj &obj);

    void debug() { postOrderDebug(root); }

  private:
    enum Direction { LEFT, RIGHT, SIBLING };

    Direction minimalBoundingBox(const std::unique_ptr<Node> &node,
                                 const Obj &obj);

    NodeP createNode(NodeP &child, const Obj &obj);

    void recursiveInsert(NodeP &node, const NodeP &parent, const Obj &obj);

    void postOrderDebug(const NodeP &node);
  };

  Tree tree;
};

#endif // !LIB
