#include <algorithm>
#include "lib.hpp"

using namespace std;

Obj boxUnion(const Obj &o1, const Obj &o2) {
  Obj o;
  o.p = {min(o1.p.x, o2.p.x), min(o1.p.y, o2.p.y)};
  o.h = max(o1.p.y + o1.h, o2.p.y + o2.h) - o.p.y;
  o.w = max(o1.p.x + o1.w, o2.p.x + o2.w) - o.p.x;

  return o;
}
