#pragma once

#include <string>
#include <vector>

class Edge;

class Node {
 public:
  explicit Node(const std::string& id): id_(std::move(id)) {}

  void AddInEdge(Edge* input_edge);
  void AddOutEdge(Edge* output_edge);
  void RemoveInEdge(Edge* input_edge);
  void RemoveOutEdge(Edge* output_edge);
  void RemoveEdge(Edge* edge);

  const std::string&        GetId() const noexcept { return id_; }
  const std::vector<Edge*>& GetInputVec() const noexcept { return input_; }
  const std::vector<Edge*>& GetOutputVec() const noexcept { return output_; }

 private:
  std::string        id_;
  std::vector<Edge*> input_;
  std::vector<Edge*> output_;

  void AddEdgeTo(std::vector<Edge*>& vec, Edge* edge);
  void RemoveEdgeTo(std::vector<Edge*>& vec, Edge* edge);
};
