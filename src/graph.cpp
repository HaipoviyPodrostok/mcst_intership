#include "graph.hpp"

#include <memory>
#include <stdexcept>
#include <string>

#include "edge.hpp"

void Graph::AddNode(const std::string& id) {
  if (nodes_.contains(id)) { return; }

  nodes_[id] = std::make_unique<Node>(id);
}

void Graph::AddEdge(const std::string& src_id, const std::string& dest_id,
                    const size_t weight) {
  const bool has_src  = nodes_.contains(src_id);
  const bool has_dest = nodes_.contains(dest_id);

  if (!has_src && !has_dest) {
    throw std::invalid_argument("Unknown nodes " + src_id + " " + dest_id);
  }

  if (!has_src) { throw std::invalid_argument("Unknown node " + src_id); }

  if (!has_dest) { throw std::invalid_argument("Unknown node " + dest_id); }

  Node* src_node  = nodes_.at(src_id).get();
  Node* dest_node = nodes_.at(dest_id).get();

  auto new_edge = std::make_unique<Edge>(src_node, dest_node, weight);
  edges_.push_back(std::move(new_edge));

  Edge* new_edge_ptr = new_edge.get();

  src_node->AddInEdge(new_edge_ptr);
  src_node->AddOutEdge(new_edge_ptr);
}

void Graph::RemoveEdge(const std::string& src_id, const std::string& dest_id) {
  const bool has_src  = nodes_.contains(src_id);
  const bool has_dest = nodes_.contains(dest_id);

  if (!has_src && !has_dest) {
    throw std::invalid_argument("Unknown nodes " + src_id + " " + dest_id);
  }

  if (!has_src) { throw std::invalid_argument("Unknown node " + src_id); }

  if (!has_dest) { throw std::invalid_argument("Unknown node " + dest_id); }
}

void Graph::RemoveNode(const std::string& id) {
  if (!nodes_.contains(id)) { return; }
}
