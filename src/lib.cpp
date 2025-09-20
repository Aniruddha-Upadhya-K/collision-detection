#include <algorithm>
#include <iostream>
#include <array>
#include "lib.hpp"

// Vec2 class methods definition
Vec2::Vec2(int x, int y) : val({x, y}) {};

bool Vec2::operator==(const Vec2 &other) const {
  return (this->val == other.val);
}

bool Vec2::operator!=(const Vec2 &other) const { return !(*this == other); }

Vec2 Vec2::operator+(const Vec2 &other) const {
  return {this->val[0] + other[0], this->val[1] + other[1]};
}

Vec2 Vec2::operator-(const Vec2 &other) const {
  return {this->val[0] - other.val[0], this->val[1] - other.val[1]};
}

int Vec2::operator[](std::size_t idx) const {
  if (idx > 1 || idx < 0)
    throw "Vec2: accessing out of bound index";
  return this->val[idx];
}

std::ostream &operator<<(std::ostream &stream, const Vec2 &vec) {
  stream << '(' << vec[0] << ", " << vec[1] << ')';
  return stream;
}

Vec2 Vec2::min(const Vec2 &vec1, const Vec2 &vec2) {
  return {std::min(vec1[0], vec2[0]), std::min(vec1[1], vec2[1])};
}

Vec2 Vec2::max(const Vec2 &vec1, const Vec2 &vec2) {
  return {std::max(vec1[0], vec2[0]), std::max(vec1[1], vec2[1])};
}

// BBox class methods definitions
BBox::BBox(const Vec2 &pos, const int width, const int height, int id)
    : _min_pt(pos), _max_pt(pos + Vec2{width, height}), id(id) {};

BBox::BBox(const Vec2 &min_pt, const Vec2 &max_pt)
    : _min_pt(min_pt), _max_pt(max_pt), id() {};

double BBox::area() const {
  Vec2 dist = _max_pt - _min_pt;
  return dist[0] * dist[1];
}

const BBox BBox::boundingBoxUnion(const BBox &bbox1, const BBox &bbox2) {
  return {Vec2::min(bbox1._min_pt, bbox2._min_pt),
          Vec2::max(bbox1._max_pt, bbox2._max_pt)};
}

bool BBox::isBoundingBoxIntersection(const BBox &bbox1, const BBox &bbox2) {
  bool intersection_x = bbox1._min_pt[0] <= bbox2._max_pt[0] &&
                        bbox1._max_pt[0] >= bbox2._min_pt[0];

  bool intersection_y = bbox1._min_pt[1] <= bbox2._max_pt[1] &&
                        bbox1._max_pt[1] >= bbox2._min_pt[1];

  return intersection_x && intersection_y;
}

bool BBox::operator==(const BBox &other) const {
  return (this->id == other.id && this->_max_pt == other._max_pt &&
          this->_min_pt == other._min_pt);
}

bool BBox::operator!=(const BBox &other) const { return !(*this == other); }

std::ostream &operator<<(std::ostream &stream, const BBox &bbox) {
  stream << "Object " << bbox.id << std::endl;
  stream << "min = " << bbox._min_pt << std::endl;
  stream << "max = " << bbox._max_pt << std::endl;
  return stream;
}

