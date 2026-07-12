#include "node.hpp"

#include <algorithm>
#include <cassert>

void Node::AddEdgeTo(std::vector<Edge*>& vec, Edge* edge) {
  assert(edge);
  vec.push_back(edge);
}

void Node::AddInEdge(Edge* input_edge) { AddEdgeTo(input_, input_edge); }

void Node::AddOutEdge(Edge* output_edge) { AddEdgeTo(output_, output_edge); }

void Node::RemoveEdgeTo(std::vector<Edge*>& vec, Edge* edge) {
  assert(edge);

  auto it = std::find(vec.begin(), vec.end(), edge);
  if (it != vec.end()) {
    std::iter_swap(it, vec.end() - 1);
    vec.pop_back();
  }
}

void Node::RemoveInEdge(Edge* input_edge) { RemoveEdgeTo(input_, input_edge); }

void Node::RemoveOutEdge(Edge* output_edge) { RemoveEdgeTo(output_, output_edge); }

void Node::RemoveEdge(Edge* edge) {
  RemoveInEdge(edge);
  RemoveOutEdge(edge);
}
