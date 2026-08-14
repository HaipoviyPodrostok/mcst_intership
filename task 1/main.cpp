#include <cstdint>
#include <iostream>
#include <string>

#include "graph.hpp"
#include "graph_algo.hpp"

#ifdef ENABLE_DUMP
  #include <filesystem>
  #include <fstream>

inline void GraphDump(const Graph& graph) {
  static uint64_t   dump_counter = 1;
  const std::string dir_path     = "dumps/dots";

  if (!std::filesystem::exists(dir_path)) {
    std::filesystem::create_directories(dir_path);
  }
  std::string filename =
    dir_path + "/graph_" + std::to_string(dump_counter++) + ".dot";
  std::ofstream file(filename);
  if (file) { DumpToGraphviz(graph, file); }
}
#endif  // ENABLE_DUMP

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);

  Graph       graph;
  std::string command;

  while (std::cin >> command) {
    try {
      if (command == "NODE") {
        std::string id;
        std::cin >> id;
        graph.AddNode(id);
      }
      else if (command == "EDGE") {
        std::string from, to;
        uint64_t    weight;
        std::cin >> from >> to >> weight;
        graph.AddEdge(from, to, weight);
      }
      else if (command == "REMOVE") {
        std::string sub_command;
        std::cin >> sub_command;

        if (sub_command == "NODE") {
          std::string id;
          std::cin >> id;
          graph.RemoveNode(id);
        }
        else if (sub_command == "EDGE") {
          std::string from, to;
          std::cin >> from >> to;
          graph.RemoveEdge(from, to);
        }
      }

#ifdef ENABLE_DUMP
      else if (command == "DUMP") { GraphDump(graph); }
#endif  // ENABLE_DUMP

      else if (command == "RPO_NUMBERING") {
        std::string id;
        std::cin >> id;
        
        auto result = RunRpoNumbering(graph, id);
        
        for (const auto& loop : result.loops) {
          std::cout << "Found loop " << loop.first << "->" << loop.second << "\n";
        }
        for (size_t i = 0; i < result.post_order.size(); ++i) {
          if (i > 0) std::cout << " ";
          std::cout << result.post_order[i];
        }
        std::cout << "\n";
      }
      else if (command == "DIJKSTRA") {
        std::string id;
        std::cin >> id;
        
        auto results = RunDijkstra(graph, id);
        
        for (const auto& [node, dist] : results) {
          std::cout << node << " " << dist << "\n";
        }
      }
      else if (command == "MAXFLOW") {
        std::string from, to;
        std::cin >> from >> to;
        
        std::cout << RunMaxFlow(graph, from, to) << "\n";
      }
      else if (command == "TARJAN") {
        std::string id;
        std::cin >> id;
        
        auto sccs = RunTarjan(graph, id);
        
        for (const auto& scc : sccs) {
          for (size_t i = 0; i < scc.size(); ++i) {
            if (i > 0) std::cout << " ";
            std::cout << scc[i];
          }
          std::cout << "\n";
        }
      }
    }
    catch (const std::exception& e) {
      std::cout << e.what() << "\n";
    }
  }

  return 0;
}