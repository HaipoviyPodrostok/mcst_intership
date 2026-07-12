#pragma once

#include <string>

class Graph;

void RunRpoNumbering(const Graph& graph, const std::string& start_id);
void RunDijkstra(const Graph& graph, const std::string& start_id);
void RunMaxFlow(const Graph& graph, const std::string& source,
                const std::string& sink);
void RunTarjan(const Graph& graph, const std::string& start_id);
