#include <iostream>
#include <string>
#include <cstdint>

#include "graph.hpp"

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);

  Graph       graph;
  std::string command;

  while (std::cin >> command) {
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
    // else if (command == "RPO_NUMBERING") {
    //   std::string id;
    //   std::cin >> id;
    //   graph.RunRpoNumbering(id);
    // }
    // else if (command == "DIJKSTRA") {
    //   std::string id;
    //   std::cin >> id;
    //   graph.RunDijkstra(id);
    // }
    // else if (command == "MAXFLOW") {
    //   std::string from, to;
    //   std::cin >> from >> to;
    //   graph.RunMaxFlow(from, to);
    // }
    // else if (command == "TARJAN") {
    //   std::string id;
    //   std::cin >> id;
    //   graph.RunTarjan(id);
    // }
  }

  return 0;
}