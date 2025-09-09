#include "lib.hpp"

int main() {
  std::vector<Obj> objs {{{0,0}, 5, 5}, {{6,4}, 2, 2}, {{3, 10}, 5, 5}, {{2,6}, 2, 2}};
  Collider col(objs); 

  col.debug();

  return 0;
}
