#include "graph_algo.hpp"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <limits>
#include <queue>
#include <unordered_map>
#include <vector>

#include "edge.hpp"
#include "graph.hpp"
#include "node.hpp"

namespace {

enum class DfsColor {
  White,
  Gray,
  Black
};

void PostOrderDfs(const Graph& graph, const std::string& id,
                  std::unordered_map<std::string, DfsColor>& color,
                  std::vector<std::string>&                  post_order) {
  color[id] = DfsColor::Gray;

  Node*              node         = graph.GetNodes().at(id).get();
  std::vector<Edge*> sorted_edges = node->GetOutputVec();

  std::sort(sorted_edges.begin(), sorted_edges.end(),
            [](const Edge* a, const Edge* b) {
              return a->GetDest()->GetId() < b->GetDest()->GetId();
            });

  for (Edge* edge : sorted_edges) {
    const std::string& dest_id = edge->GetDest()->GetId();

    if (color[dest_id] == DfsColor::Gray) {
      std::cout << "Found loop " << id << "->" << dest_id << "\n";
    }
    else if (color[dest_id] == DfsColor::White) {
      PostOrderDfs(graph, dest_id, color, post_order);
    }
  }

  color[id] = DfsColor::Black;
  post_order.push_back(id);
}

struct TarjanState {
  int                                   index_counter = 0;
  std::unordered_map<std::string, int>  node_index;
  std::unordered_map<std::string, int>  lowlink;
  std::unordered_map<std::string, bool> on_stack;
  std::vector<std::string>              stack;
  std::vector<std::vector<std::string>> sccs;
};

void TarjanDfs(const Graph& graph, const std::string& id, TarjanState& state) {
  state.node_index[id] = state.index_counter;
  state.lowlink[id]    = state.index_counter;
  state.index_counter++;
  state.stack.push_back(id);
  state.on_stack[id] = true;

  Node*              node         = graph.GetNodes().at(id).get();
  std::vector<Edge*> sorted_edges = node->GetOutputVec();
  
  std::sort(sorted_edges.begin(), sorted_edges.end(),
            [](const Edge* a, const Edge* b) {
              return a->GetDest()->GetId() < b->GetDest()->GetId();
            });

  for (Edge* edge : sorted_edges) {
    const std::string& dest = edge->GetDest()->GetId();
  
    if (state.node_index[dest] == -1) {
      TarjanDfs(graph, dest, state);
      state.lowlink[id] = std::min(state.lowlink[id], state.lowlink[dest]);
    }
  
    else if (state.on_stack[dest]) {
      state.lowlink[id] = std::min(state.lowlink[id], state.node_index[dest]);
    }
  }

  if (state.lowlink[id] == state.node_index[id]) {
    std::vector<std::string> scc;
    
    while (true) {
      std::string w = state.stack.back();
      state.stack.pop_back();
      state.on_stack[w] = false;
      scc.push_back(w);
      if (w == id) { break; }
    }
    
    state.sccs.push_back(std::move(scc));
  }
}

}  // namespace

void RunRpoNumbering(const Graph& graph, const std::string& start_id) {
  graph.CheckNodesExist(start_id);

  std::unordered_map<std::string, DfsColor> color;
  for (const auto& [id, _] : graph.GetNodes()) { color[id] = DfsColor::White; }

  std::vector<std::string> post_order;

  PostOrderDfs(graph, start_id, color, post_order);

  std::reverse(post_order.begin(), post_order.end());

  for (size_t i = 0; i < post_order.size(); ++i) {
    if (i > 0) std::cout << " ";
    std::cout << post_order[i];
  }
  std::cout << "\n";
}

void RunDijkstra(const Graph& graph, const std::string& start_id) {
  graph.CheckNodesExist(start_id);

  const auto& nodes = graph.GetNodes();

  std::unordered_map<std::string, uint64_t> distances;
  for (const auto& [id, _] : nodes) {
    distances[id] = std::numeric_limits<uint64_t>::max();
  }
  distances[start_id] = 0;

  using P = std::pair<uint64_t, std::string>;
  std::priority_queue<P, std::vector<P>, std::greater<P>> pq;
  pq.push({0, start_id});

  while (!pq.empty()) {
    auto              pq_top   = pq.top();
    const uint64_t    dist     = pq_top.first;
    const std::string cur_node = pq_top.second;

    pq.pop();

    if (dist > distances[cur_node]) { continue; }

    Node* node = nodes.at(cur_node).get();

    const std::vector<Edge*>& node_output = node->GetOutputVec();

    for (const Edge* edge : node_output) {
      const std::string& neighbour = edge->GetDest()->GetId();
      const uint64_t     weight    = edge->GetWeight();

      if (distances[cur_node] + weight < distances[neighbour]) {
        distances[neighbour] = distances[cur_node] + weight;
        pq.push({distances[neighbour], neighbour});
      }
    }
  }

  std::vector<std::pair<std::string, uint64_t>> results;

  for (const auto& [id, dist] : distances) {
    if (id != start_id && dist != std::numeric_limits<uint64_t>::max()) {
      results.push_back({id, dist});
    }
  }

  std::sort(results.begin(), results.end());

  for (const auto& [id, dist] : results) { std::cout << id << " " << dist << "\n"; }
}

void RunMaxFlow(const Graph& graph, const std::string& source,
                const std::string& sink) {
  graph.CheckNodesExist(source, sink);

  const auto& nodes = graph.GetNodes();

  using ResidualMap =
    std::unordered_map<std::string, std::unordered_map<std::string, uint64_t>>;
  ResidualMap residual;

  for (const auto& [id, node_ptr] : nodes) {
    for (const Edge* edge : node_ptr->GetOutputVec()) {
      const std::string& dest = edge->GetDest()->GetId();      
      residual[id][dest] += edge->GetWeight();
      residual[dest][id];
    }
  }

  uint64_t max_flow = 0;

  while (true) {
    std::unordered_map<std::string, std::string> parent;
    std::queue<std::string>                      bfs_queue;
    parent[source] = source;
    bfs_queue.push(source);

    while (!bfs_queue.empty() && parent.find(sink) == parent.end()) {
      std::string u = bfs_queue.front();
      bfs_queue.pop();

      for (const auto& [v, cap] : residual[u]) {
        if (parent.find(v) == parent.end() && cap > 0) {
          parent[v] = u;
          bfs_queue.push(v);
        }
      }
    }

    if (parent.find(sink) == parent.end()) { break; }

    uint64_t path_flow = std::numeric_limits<uint64_t>::max();
    for (std::string v = sink; v != source; v = parent[v]) {
      path_flow = std::min(path_flow, residual[parent[v]][v]);
    }

    for (std::string v = sink; v != source; v = parent[v]) {
      residual[parent[v]][v] -= path_flow;
      residual[v][parent[v]] += path_flow;
    }

    max_flow += path_flow;
  }

  std::cout << max_flow << "\n";
}

void RunTarjan(const Graph& graph, const std::string& start_id) {
  graph.CheckNodesExist(start_id);

  const auto& nodes = graph.GetNodes();

  TarjanState state;
  
  for (const auto& [id, _] : nodes) {
    state.node_index[id] = -1;
    state.on_stack[id]   = false;
  }

  TarjanDfs(graph, start_id, state);

  for (const auto& [id, _] : nodes) {
    if (state.node_index[id] == -1) { TarjanDfs(graph, id, state); }
  }

  for (auto& scc : state.sccs) {
    if (scc.size() > 1) {
      std::sort(scc.begin(), scc.end());
      for (size_t i = 0; i < scc.size(); ++i) {
        if (i > 0) std::cout << " ";
        std::cout << scc[i];
      }
      std::cout << "\n";
    }
  }
}
