#include "graph.hpp"

#include <algorithm>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include "edge.hpp"
#include "node.hpp"

void Graph::AddNode(const std::string& id) {
  if (nodes_.contains(id)) { return; }

  nodes_[id] = std::make_unique<Node>(id);
}

void Graph::AddEdge(const std::string& src_id, const std::string& dest_id,
                    const uint64_t weight) {
  CheckNodesExist(src_id, dest_id);

  Node* src_node  = nodes_.at(src_id).get();
  Node* dest_node = nodes_.at(dest_id).get();

  auto  new_edge     = std::make_unique<Edge>(src_node, dest_node, weight);
  Edge* new_edge_ptr = new_edge.get();

  edges_.push_back(std::move(new_edge));

  src_node->AddOutEdge(new_edge_ptr);
  dest_node->AddInEdge(new_edge_ptr);
}

void Graph::RemoveEdge(const std::string& src_id, const std::string& dest_id) {
  CheckNodesExist(src_id, dest_id);

  const auto src_it  = nodes_.find(src_id);
  const auto dest_it = nodes_.find(dest_id);

  Node* src  = src_it->second.get();
  Node* dest = dest_it->second.get();

  const std::vector<Edge*>& src_output = src->GetOutputVec();

  auto local_edge_it =
    std::find_if(src_output.begin(), src_output.end(),
                 [dest](Edge* edge) { return edge->GetDest() == dest; });

  if (local_edge_it != src_output.end()) {
    Edge* edge = *local_edge_it;

    RemoveEdgeByPtr(edge);
  }
}

void Graph::RemoveEdgeByPtr(Edge* edge) {
  assert(edge);

  Node* src  = edge->GetSrc();
  Node* dest = edge->GetDest();

  src->RemoveOutEdge(edge);
  dest->RemoveInEdge(edge);

  auto global_edge_it = std::find_if(edges_.begin(), edges_.end(),
                                     [edge](const std::unique_ptr<Edge>& curr_edge) {
                                       return curr_edge.get() == edge;
                                     });

  if (global_edge_it != edges_.end()) {
    std::swap(*global_edge_it, edges_.back());
    edges_.pop_back();
  }
}

void Graph::RemoveNode(const std::string& id) {
  CheckNodesExist(id);

  Node* node_to_remove = nodes_.at(id).get();

  const std::vector<Edge*> input  = node_to_remove->GetInputVec();
  const std::vector<Edge*> output = node_to_remove->GetOutputVec();

  for (Edge* e : input) { RemoveEdgeByPtr(e); }
  for (Edge* e : output) { RemoveEdgeByPtr(e); }

  nodes_.erase(id);
}

void Graph::CheckNodesExist(const std::string& id) const {
  if (!nodes_.contains(id)) { throw std::invalid_argument("Unknown node " + id); }
}

void Graph::CheckNodesExist(const std::string& src_id,
                            const std::string& dest_id) const {
  const bool has_src  = nodes_.contains(src_id);
  const bool has_dest = nodes_.contains(dest_id);

  if (!has_src && !has_dest) {
    throw std::invalid_argument("Unknown nodes " + src_id + " " + dest_id);
  }

  if (!has_src) { throw std::invalid_argument("Unknown node " + src_id); }

  if (!has_dest) { throw std::invalid_argument("Unknown node " + dest_id); }
}

#ifdef ENABLE_DUMP
void DumpToGraphviz(const Graph& graph, std::ostream& out) {
  out << "digraph G {\n";
  const auto& nodes = graph.GetNodes();
  
  for (const auto& [id, node_ptr] : nodes) {
    out << "    \"" << id << "\";\n";
    
    for (const Edge* edge : node_ptr->GetOutputVec()) {
      out << "    \"" << id << "\" -> \"" << edge->GetDest()->GetId() << "\" "
          << "[label=\"" << edge->GetWeight() << "\"];\n";
    }
  }
  
  out << "}\n";
}
#endif