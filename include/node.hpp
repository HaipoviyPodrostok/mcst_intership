#pragma once

#include <string>
#include <vector>

class Edge;

class Node {
 public:
  explicit Node(const std::string& id): id_(std::move(id)) {}

  void AddInEdge(Edge* input_edge);
  void AddOutEdge(Edge* output_edge);
  void RemoveInEdgde(Edge* input_edge);
  void RemoveOutEdge(Edge* output_edge);

 private:
  std::string id_;

  std::vector<Edge*> input_;
  std::vector<Edge*> output_;

  void AddEdgeTo(std::vector<Edge*>& vec, Edge* edge);
  void RemoveEdgeTo(std::vector<Edge*>& vec, Edge* edge);
};
