#include "pch.h"
#include "Hungary.h"


TEST(HungarianAlgorithmTest, SimpleBipartiteGraph) {
    HungarianAlgorithm hungarian;
    hungarian.Initialize(2, 2);
    hungarian.AddAdjacency(1, 1);
    hungarian.AddAdjacency(2, 2);
    ASSERT_EQ(hungarian.Solve(), 2);
    hungarian.Clear();
}

TEST(HungarianAlgorithmTest, UnmatchedNodes) {
    HungarianAlgorithm hungarian;
    hungarian.Initialize(3, 3);
    hungarian.AddAdjacency(1, 1);
    hungarian.AddAdjacency(2, 2);
    ASSERT_EQ(hungarian.Solve(), 2);
    hungarian.Clear();
}

TEST(HungarianAlgorithmTest, MultipleMatches) {
    HungarianAlgorithm hungarian;
    hungarian.Initialize(3, 3);
    hungarian.AddAdjacency(1, 1);
    hungarian.AddAdjacency(1, 2);
    hungarian.AddAdjacency(2, 2);
    hungarian.AddAdjacency(3, 3);
    ASSERT_EQ(hungarian.Solve(), 3);
    hungarian.Clear();
}

TEST(HungarianAlgorithmTest, EmptyGraph) {
    HungarianAlgorithm hungarian;
    hungarian.Initialize(0, 0);
    ASSERT_EQ(hungarian.Solve(), 0);
    hungarian.Clear();
}

TEST(HungarianAlgorithmTest, NonSquareGraph) {
    HungarianAlgorithm hungarian;
    hungarian.Initialize(2, 3);
    hungarian.AddAdjacency(1, 1);
    hungarian.AddAdjacency(2, 2);
    hungarian.AddAdjacency(2, 3);
    ASSERT_EQ(hungarian.Solve(), 2);
    hungarian.Clear();
}

TEST(HungarianAlgorithmTest, ComplexBipartiteGraph) {
    HungarianAlgorithm hungarian;
    hungarian.Initialize(10, 6);
    hungarian.AddAdjacency(1, 1);
    hungarian.AddAdjacency(1, 2);
    hungarian.AddAdjacency(2, 2);
    hungarian.AddAdjacency(2, 3);
    hungarian.AddAdjacency(3, 3);
    hungarian.AddAdjacency(4, 1);
    hungarian.AddAdjacency(4, 4);
    hungarian.AddAdjacency(5, 4);
    hungarian.AddAdjacency(5, 5);
    hungarian.AddAdjacency(6, 5);
    hungarian.AddAdjacency(7, 5);
    hungarian.AddAdjacency(7, 6);
    hungarian.AddAdjacency(8, 6);
    hungarian.AddAdjacency(9, 1);
    hungarian.AddAdjacency(10, 6);
    ASSERT_EQ(hungarian.Solve(), 6);
    hungarian.Clear();
}