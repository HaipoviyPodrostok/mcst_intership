#include <gtest/gtest.h>
#include <stdexcept>
#include "graph.hpp"

TEST(GraphTest, AddAndRemoveNodes) {
    Graph graph;
    graph.AddNode("A");
    graph.AddNode("B");

    const auto& nodes = graph.GetNodes();
    EXPECT_TRUE(nodes.contains("A"));
    EXPECT_TRUE(nodes.contains("B"));
    EXPECT_EQ(nodes.size(), 2);

    graph.RemoveNode("A");
    EXPECT_FALSE(nodes.contains("A"));
    EXPECT_TRUE(nodes.contains("B"));
    EXPECT_EQ(nodes.size(), 1);
}

TEST(GraphTest, AddEdges) {
    Graph graph;
    graph.AddNode("A");
    graph.AddNode("B");
    graph.AddEdge("A", "B", 10);

    const auto& nodes = graph.GetNodes();
    EXPECT_EQ(nodes.at("A")->GetOutputVec().size(), 1);
    EXPECT_EQ(nodes.at("B")->GetInputVec().size(), 1);
    EXPECT_EQ(nodes.at("A")->GetOutputVec()[0]->GetWeight(), 10);
}

TEST(GraphTest, InvalidEdgeThrowsException) {
    Graph graph;
    graph.AddNode("A");

    EXPECT_THROW(graph.AddEdge("A", "B", 10), std::invalid_argument);
    EXPECT_THROW(graph.AddEdge("B", "A", 10), std::invalid_argument);
    EXPECT_THROW(graph.AddEdge("X", "Y", 10), std::invalid_argument);
}
