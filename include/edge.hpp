#pragma once

#include <cassert>
#include <cstddef>

#include "node.hpp"

class Edge {
 public:
  explicit Edge(Node* src, Node* dest, const size_t weight)
    : src_(src), dest_ {dest}, weight_(weight) {
    assert(src || dest);
  }

 private:
  Node* src_;
  Node* dest_;

  size_t weight_;
};
