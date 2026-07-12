#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "edge.hpp"
#include "node.hpp"

class Graph {
 public:
  void AddNode(const std::string& id);
  void AddEdge(const std::string& src, const std::string& dest,
               const uint64_t weight);
  void RemoveNode(const std::string& id);
  void RemoveEdge(const std::string& src, const std::string& dest);

  void CheckNodesExist(const std::string& id) const;
  void CheckNodesExist(const std::string& src_id, const std::string& dest_id) const;

  const std::unordered_map<std::string, std::unique_ptr<Node>>& GetNodes()
    const noexcept {
    return nodes_;
  }

 private:
  std::unordered_map<std::string, std::unique_ptr<Node>> nodes_;
  std::vector<std::unique_ptr<Edge>>                     edges_;

  void RemoveEdgeByPtr(Edge* edge);
};

#ifdef ENABLE_DUMP
void DumpToGraphviz(const Graph& graph, std::ostream& out);
#endif