#pragma once

#include <cassert>
#include <cstdint>

#include "node.hpp"

class Edge {
 public:
  explicit Edge(Node* src, Node* dest, const uint64_t weight);

  Node*    GetSrc() const noexcept { return src_; }
  Node*    GetDest() const noexcept { return dest_; }
  uint64_t GetWeight() const noexcept { return weight_; }

 private:
  Node* src_;
  Node* dest_;

  uint64_t weight_;
};
