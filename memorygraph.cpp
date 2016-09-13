#include <iostream>
#include <vector>
#include <iterator>
#include <unordered_map>
#include <unordered_set>
#include <cstdint>
#include "memorygraph.hpp"

using std::unordered_map;
using std::unordered_set;
using std::vector;
using std::iterator;
using std::cout;
using std::pair;

static unordered_map<uint64_t,vector<uint64_t> > adjacencyList;

static unordered_set<uint64_t> nodes;

struct nodeData {
	bool in_graph;
	int status;
};

struct neighborData{
	vector<uint64_t> neighbors;
	int status;
};

struct distanceData {
	uint64_t distance;
	int status;
};

int add_node(uint64_t node_id) {
	if(get_node(node_id).in_graph) {
		// Node already in graph
		return 204;
	} else {
		// Node not already in graph
		nodes.insert(node_id);
		return 200;
	}
}

int add_edge(uint64_t node_a_id, uint64_t node_b_id) {
	if(node_a_id == node_b_id) {
		// Nodes A and B are the same
		return 400;
	}

	struct nodeData edgeData = get_edge(node_a_id,node_b_id);
	if(edgeData.status == 400) {
		// A or B is not in the graph
		return 400;
	} else if (edgeData.in_graph) {
		// Edge already exists in the graph
		return 204;
	} else {
		// Add the edge to the adjacency list
		adjacencyList[node_a_id].push_back(node_b_id);
		adjacencyList[node_b_id].push_back(node_a_id);
		return 200;
	}
}

int remove_node(uint64_t node_id) {
	unordered_set<uint64_t>::const_iterator found = nodes.find(node_id);

	if(found != nodes.end()){
		// Remove node from set of nodes
		nodes.erase(node_id);

		// Remove all edges with node
		unordered_map<uint64_t,vector<uint64_t> >::const_iterator foundEdges = adjacencyList.find(node_id);

		if(foundEdges != adjacencyList.end()) {
			vector<uint64_t> adjacentNodes = foundEdges->second;

			// Remove from the adjacency list of all adjacent nodes
			for(uint64_t newNode : adjacentNodes){
				remove_edge(newNode,node_id);
			}

			// Clear adjacency list for node_id itself
			adjacencyList[node_id].clear();
		}
		return 200;
	} else {
		// Node does not exist
		return 400;
	}
}

int remove_edge(uint64_t node_a_id, uint64_t node_b_id) {
	unordered_map<uint64_t,vector<uint64_t> >::const_iterator foundA = adjacencyList.find(node_a_id);
	unordered_map<uint64_t,vector<uint64_t> >::const_iterator foundB = adjacencyList.find(node_b_id);
	if(foundA != adjacencyList.end() && foundB != adjacencyList.end()){
		bool adjacencyFound = false;

		// Remove B from A's adjacency list
		vector<uint64_t> edgesFromA = foundA->second;
		for(vector<uint64_t>::const_iterator i = edgesFromA.begin(); i != edgesFromA.end(); i++) {
			if((*i) == node_b_id){
				adjacencyList[node_a_id].erase(i);
				adjacencyFound = true;
				break;
			}
		}

		// Remove A from B's adjacency list
		vector<uint64_t> edgesFromB = foundB->second;
		for(vector<uint64_t>::const_iterator i = edgesFromB.begin(); i != edgesFromB.end(); i++) {
			if((*i) == node_a_id){
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

struct nodeData get_node(uint64_t node_id) {
	struct nodeData out;
	out.status = 200;
	unordered_set<uint64_t>::const_iterator found = nodes.find(node_id);

	if(found == nodes.end()){
		// Node not in graph
		out.in_graph = false;
	} else {
		// Node in graph
		out.in_graph = true;
	}

	return out;
}

struct nodeData get_edge(uint64_t node_a_id, uint64_t node_b_id) {
	struct nodeData out;
	unordered_set<uint64_t>::const_iterator foundA = nodes.find(node_a_id);
	unordered_set<uint64_t>::const_iterator foundB = nodes.find(node_b_id);

	if(foundA == nodes.end() || foundB == nodes.end()) {
		// A or B not in graph
		out.status = 400;
		out.in_graph = false;
	} else {
		out.status = 200;

		// A and B in graph
		vector<uint64_t> adjacentToA = adjacencyList.find(node_a_id)->second;
		bool nodesAdjacent = false;

		for(uint64_t node : adjacentToA) {
			if(node == node_b_id) {
				nodesAdjacent = true;
				break;
			}
		}
		out.in_graph = nodesAdjacent;
	}

	return out;
}

struct neighborData get_neighbors(uint64_t node_id) {
	struct neighborData out;
	unordered_map<uint64_t,vector<uint64_t> >::const_iterator found = adjacencyList.find(node_id);
	if(found != adjacencyList.end()) {
		// Found neighbors for node
		out.neighbors = adjacencyList[node_id];
		out.status = 200;
	} else {
		// No neighbors found for node
		out.neighbors.clear();
		out.status = 400;
	}
	return out;
}

int shortest_path(uint64_t node_a_id, uint node_b_id) {}

void print_nodes(){
	for(uint64_t node : nodes) {
		cout << node << '\n';
	}
}

void print_graph(){
	for(pair<uint64_t,vector<uint64_t> > pairing : adjacencyList) {
		cout << pairing.first << ": ";
		for(uint64_t adjacency : pairing.second) {
			cout << adjacency << ' ';
		}
		cout << '\n';
	}
}

int main(void) {
	cout << add_node(10) << '\n';
	cout << add_node(15) << '\n';
	cout << add_node(10) << '\n';
	cout << add_edge(10,10) << '\n';
	cout << add_edge(10,15) << '\n';
	print_nodes();
	print_graph();
	cout << remove_node(15) << '\n';
	cout << remove_node(20) << '\n';
	cout << "here" << '\n';

	cout << get_node(10).in_graph << '\n';
	cout << get_node(10).status << '\n';
	cout << get_node(15).in_graph << '\n';
	cout << get_node(15).status << '\n';
	print_nodes();
}