#ifndef LIB
#define LIB
#include <cstddef>

struct Pos {
  public:
    int x, y;
    Pos(): x(0), y(0) {};
    Pos(int x, int y): x(x), y(y) {};

    void update(int deltaX, int deltaY) {
      x += deltaX;
      y += deltaY;
    }
};

struct Obj {
    Pos p;
    int h;
    int w;
    int a;

    Obj(): p({}), h(0), w(0), a(0) {};
    Obj(Pos pos, int height, int width): p(pos), h(height), w(width), a(height * width) {};
};

struct Node {
  Obj obj;
  Node *left;
  Node *right;

  Node(): obj(), left(NULL), right(NULL) {};
  Node(Obj obj, Node *left, Node *right): obj(obj), left(left), right(right) {}
};

Obj boxUnion(const Obj &o1, const Obj &o2);

#endif // !LIB
