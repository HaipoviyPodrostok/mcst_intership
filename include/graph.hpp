#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "edge.hpp"
#include "node.hpp"

class Graph {
 public:
  void AddNode(const std::string& id);
  void AddEdge(const std::string& src, const std::string& dest, const size_t weight);
  void RemoveNode(const std::string& id);
  void RemoveEdge(const std::string& src, const std::string& dest);

 private:
  std::unordered_map<std::string, std::unique_ptr<Node>> nodes_;
  std::vector<std::unique_ptr<Edge>>                     edges_;
};