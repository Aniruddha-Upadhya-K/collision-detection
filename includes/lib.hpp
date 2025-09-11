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

  int getX() const;
  void setX(int x);

  int getY() const;
  void setY(int y);

  const std::array<int, 2> xy() const;

  bool operator==(const Pos &other) const;
  bool operator!=(const Pos &other) const;

private:
  int _x, _y;
};

class Obj {
  // TODO: copy/move constructor needed
public:
  Obj();
  Obj(const Pos &pos, const int width, const int height);

  const std::array<int, 2> pos() const;
  void pos(const Pos &pos);

  int getHeight() const;
  void setHeight(int height);

  int getWidth() const;
  void setWidth(int width);

  int area() const;

  bool operator==(const Obj &other) const;
  bool operator!=(const Obj &other) const;

  // make these two methods static
  static const Obj boundingBoxUnion(const Obj &obj1, const Obj &obj2);
  static int boundingBoxUnionArea(const Obj &obj1, const Obj &obj2);


private:
  Pos _pos;
  int _w, _h;
};

std::ostream& operator<<(std::ostream &stream, const Obj &obj);

class Collider {
public:
  Collider();
  explicit Collider(std::vector<Obj> &objects);

  std::vector<Obj> flatten(); 

private:
  struct Node {
    using NodeP = std::unique_ptr<Node>;

    Obj obj;
    NodeP left, right;

    Node();
    Node(Obj obj, NodeP &&child1, NodeP &&child2);

    explicit Node(Obj obj);

    bool isLeaf();
  };
  typedef std::unique_ptr<Node> NodeP;

  class Tree {
  public:
    std::unique_ptr<Node> root;

    Tree();
    explicit Tree(const std::vector<Obj> &objects);

    void build(const std::vector<Obj> &objects);

    void insert(const Obj &obj);

    std::vector<Obj> flatten(); 

  private:
    enum Direction { LEFT, RIGHT, SIBLING };

    Direction minimalBoundingBox(const std::unique_ptr<Node> &node,
                                 const Obj &obj);

    NodeP createNode(NodeP &child, const Obj &obj);

    void recursiveInsert(NodeP &node, const NodeP &parent, const Obj &obj);

    void postOrderFlatten(const NodeP &node, std::vector<Obj> &vec);
  };

  Tree tree;
};

#endif // !LIB
