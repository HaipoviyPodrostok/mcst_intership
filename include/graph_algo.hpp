#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

class Graph;

struct RpoResult {
  std::vector<std::string>                         post_order;
  std::vector<std::pair<std::string, std::string>> loops;
};

RpoResult RunRpoNumbering(const Graph& graph, const std::string& start_id);
std::vector<std::pair<std::string, uint64_t>> RunDijkstra(const Graph& graph,
                                                          const std::string& start_id);
uint64_t RunMaxFlow(const Graph& graph,
                    const std::string& source,
                    const std::string& sink);
std::vector<std::vector<std::string>> RunTarjan(const Graph& graph,
                                                const std::string& start_id);
