#ifndef LIB
#define LIB

#include <array>

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

#endif // !LIB
