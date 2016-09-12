#include <iostream>
#include <vector>
#include <iterator>
#include <unordered_map>
#include <unordered_set>
#include "memorygraph.hpp"

#define INITIAL_GRAPH

namespace std {
	static unordered_map<u64,vector<u64>> adjacencyList = new unordered_map<u64,vector<u64>>();

	static unordered_set<u64> nodes = new unordered_set<u64>();

	struct nodeData {
		bool in_graph;
		int status;
	};

	struct neighborData{
		vector<u64> neighbors;
		int status;
	};

	struct distanceData {
		u64 distance;
		int status;
	};

	int add_node(u64 node_id) {
		unordered_set<u64>::const_iterator found = nodes.find(node_id);

		if(found == nodes.end()){
			// Node not already in graph
			nodes.insert(node_id);
			return 200;
		} else {
			return 204;
		}
	}

	int add_edge(u64 node_a_id, u64 node_b_id) {
		if(node_a_id == node_b_id) {
			// Nodes A and B are the same
			return 400;
		}

		unordered_set<u64>::const_iterator foundA = nodes.find(node_a_id);
		unordered_set<u64>::const_iterator foundB = nodes.find(node_b_id);

		if(foundA == nodes.end() || foundB == nodes.end()){
			// A or B not already in graph
			return 400;
		} else{
			unordered_map<u64>::const_iterator foundEdge = adjacencyList.find(node_a_id);

			if(foundEdge != adjacencyList.end()) {
				vector<u64> adjacentToA = foundEdge->second;

				// Check if a is adjacent to b currently
				for(u64 node : adjacentToA){
					if(node == node_b_id) {
						return 204;
					}
				}
			}

			// Add the edge to the adjacency list
			adjacencyList[node_a_id].append(node_b_id);
			adjacencyList[node_b_id].append(node_a_id);
			return 200;
		}
	}

	int remove_node(u64 node_id) {
		unordered_set<u64>::const_iterator found = nodes.find(node_id);

		if(found != nodes.end()){
			// Remove node from set of nodes
			nodes.erase(node_id);

			// Remove all edges with node
			unordered_map<u64>::const_iterator foundEdges = adjacencyList.find(node_id);

			if(foundEdges != adjacencyList.end()) {
				vector<u64> adjacentNodes = foundEdges->second;

				// Remove from the adjacency list of all adjacent nodes
				for(u64 newNode : adjacentNodes){
					unordered_map<u64>::const_iterator edgesFromNewNodeFound = adjacencyList.find(newNode);
					if(edgesFromNewNodeFound != adjacencyList.end()){
						vector<u64> edgesFromNewNode = edgesFromNewNodeFound->second;
						for(int i = 0; i < edgesFromNewNode.size(); i++) {
							if(edgesFromNewNode[i] == node_id){
								adjacencyList[newNode].erase(i);
								break;
							}
						}
					}
				}

				// Clear adjacency list for node_id itself
				adjacencyList[node_id] = vector::empty;
			}
			return 200;
		} else {
			// Node does not exist
			return 400;
		}
	}

	int remove_edge(u64 node_a_id, u64 node_b_id) {
		unordered_map<u64>::const_iterator foundA = adjacencyList.find(node_a_id);
		unordered_map<u64>::const_iterator foundB = adjacencyList.find(node_b_id);
		if(foundA != adjacencyList.end() && foundB != adjacencyList.end()){
			bool adjacencyFound = false;

			// Remove B from A's adjacency list
			vector<u64> edgesFromA = foundA->second;
			for(int i = 0; i < edgesFromA.size(); i++) {
				if(edgesFromA[i] == node_b_id){
					adjacencyList[node_a_id].erase(i);
					adjacencyFound = true;
					break;
				}
			}

			// Remove A from B's adjacency list
			vector<u64> edgesFromB = foundB->second;
			for(int i = 0; i < edgesFromB.size(); i++) {
				if(edgesFromB[i] == node__a_id){
					adjacencyList[node_b_id].erase(i);
					adjacencyFound = true;
					break;
				}
			}

			if(adjacencyFound){
				return 200;
			}
		}
		// Edge not found
		return 400;
	}

	struct nodeData get_node(u64 node_id) {

	}

	struct nodeData get_edge(u64 node_a_id, u64 node_b_id) {

	}

	int get_neighbors(u64 node_id) {

	}

	int shortest_path(u64 node_a_id, uint node_b_id) {

	}
}