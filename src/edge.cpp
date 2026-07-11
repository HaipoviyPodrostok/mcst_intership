#include "edge.hpp"

Edge::Edge(Node* src, Node* dest, const uint64_t weight)
  : src_(src), dest_ {dest}, weight_(weight) {
  assert(src || dest);
}
