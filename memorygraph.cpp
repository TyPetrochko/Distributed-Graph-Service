#include <iostream>
#include <list>
#include <iterator>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <cstdint>
#include "memorygraph.hpp"

using std::unordered_map;
using std::unordered_set;
using std::list;
using std::iterator;
using std::cout;
using std::pair;
using std::queue;

static unordered_map<uint64_t,list<uint64_t> > adjacencyList;

static unordered_set<uint64_t> nodes;

struct traversedNode {
	uint64_t id;
	uint64_t distance;
};

int add_node(uint64_t node_id) {
	if(get_node(node_id).in_graph) {
		// Node already in graph
		return 204;
	} else {
		// Node not already in graph
		nodes.insert(node_id);
		adjacencyList[node_id];
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
		unordered_map<uint64_t,list<uint64_t> >::const_iterator foundEdges = adjacencyList.find(node_id);

		if(foundEdges != adjacencyList.end()) {
			list<uint64_t> adjacentNodes = foundEdges->second;

			// Remove from the adjacency list of all adjacent nodes
			for(uint64_t newNode : adjacentNodes){
				remove_edge(newNode,node_id);
			}

			// Clear adjacency list for node_id itself
			adjacencyList.erase(node_id);
		}
		return 200;
	} else {
		// Node does not exist
		return 400;
	}
}

int remove_edge(uint64_t node_a_id, uint64_t node_b_id) {
	unordered_map<uint64_t,list<uint64_t> >::const_iterator foundA = adjacencyList.find(node_a_id);
	unordered_map<uint64_t,list<uint64_t> >::const_iterator foundB = adjacencyList.find(node_b_id);
	if(foundA != adjacencyList.end() && foundB != adjacencyList.end()){
		bool adjacencyFound = false;

		// Remove B from A's adjacency list
		list<uint64_t> edgesFromA = foundA->second;
		for(list<uint64_t>::const_iterator i = edgesFromA.begin(); i != edgesFromA.end(); i++) {
			if((*i) == node_b_id){
				// adjacencyList[node_a_id].erase(i);
				adjacencyFound = true;
				break;
			}
		}

		// Remove A from B's adjacency list
		list<uint64_t> edgesFromB = foundB->second;
		for(list<uint64_t>::const_iterator i = edgesFromB.begin(); i != edgesFromB.end(); i++) {
			if((*i) == node_a_id){
				// adjacencyList[node_b_id].erase(i);
				adjacencyFound = true;
				break;
			}
		}

		adjacencyList[node_b_id].remove(node_a_id);
		adjacencyList[node_a_id].remove(node_b_id);

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
		list<uint64_t> adjacentToA = adjacencyList[node_a_id];
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
	unordered_map<uint64_t,list<uint64_t> >::const_iterator found = adjacencyList.find(node_id);
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

struct distanceData shortest_path(uint64_t node_a_id, uint64_t node_b_id) {
	struct distanceData out;

	if(nodes.find(node_a_id) == nodes.end() || nodes.find(node_b_id) == nodes.end()) {
		out.distance = -1;
		out.status = 400;
		return out;
	}

	if(node_a_id == node_b_id) {
		out.distance = 0;
		out.status = 200;
		return out;
	}
	queue<struct traversedNode> traversal;
	unordered_set<uint64_t> seen;

	struct traversedNode first;
	first.id = node_a_id;
	first.distance = 0;
	traversal.push(first);
	seen.insert(first.id);
	while(!traversal.empty()) {
		struct traversedNode curr = traversal.front();
		traversal.pop();
		list<uint64_t> adjacent = get_neighbors(curr.id).neighbors;

		for(uint64_t adjNode : adjacent) {
			if(adjNode == node_b_id) {
				out.distance = curr.distance + 1;
				out.status = 200;
				return out;
			}
			if(seen.find(adjNode) == seen.end()){
				struct traversedNode newNode;
				newNode.id = adjNode;
				newNode.distance = curr.distance + 1;
				traversal.push(newNode);
				seen.insert(adjNode);
			}
		}
	}

	// Nodes are not connected
	out.distance = -1;
	out.status = 204;
	return out;
}

void print_nodes(){
	for(uint64_t node : nodes) {
		cout << node << '\n';
	}
}

void print_graph(){
	for(pair<uint64_t,list<uint64_t> > pairing : adjacencyList) {
		cout << pairing.first << ": ";
		for(uint64_t adjacency : pairing.second) {
			cout << adjacency << ' ';
		}
		cout << '\n';
	}
}

int x_main(void) {
	cout << add_node(5) << '\n';
	print_graph();
	cout << '\n';
	cout << add_node(10) << '\n';
	cout << add_node(15) << '\n';
	cout << add_node(10) << '\n';
	cout << add_node(11) << '\n';
	cout << add_node(25) << '\n';
	cout << add_edge(10,10) << '\n';
	cout << add_edge(10,20) << '\n';
	cout << "I AM HERE\n";
	cout << add_edge(10,15) << '\n';
	cout << add_edge(10,15) << '\n';
	cout << add_edge(25,15) << '\n';
	cout << "I AM HERE TOO \n";
	print_nodes();
	print_graph();
	cout << "I AM HERE THREE \n";
	cout << remove_edge(25,15) << '\n';
	cout << "Removed edge: \n";
	print_graph();
	cout << '\n';
	cout << remove_edge(20,15) << '\n';
	cout << remove_edge(15,15) << '\n';
	cout << remove_edge(10,11) << '\n';
	cout << remove_edge(25,15) << '\n';
	cout << remove_node(15) << '\n';
	cout << "Removed node: \n";
	print_graph();
	cout << '\n';
	cout << remove_node(20) << '\n';
	cout << "here" << '\n';

	cout << get_node(10).in_graph << '\n';
	cout << get_node(10).status << '\n';
	cout << get_node(15).in_graph << '\n';
	cout << get_node(15).status << '\n';
	print_nodes();
	print_graph();
	cout << '\n';

	cout << add_node(15) << '\n';
	cout << add_node(20) << '\n';
	cout << add_node(30) << '\n';
	cout << add_node(35) << '\n';
	cout << add_edge(15,20) << '\n';
	cout << add_edge(20,30) << '\n';
	cout << add_edge(30,35) << '\n';
	cout << "Shortest path from 15 to 35: " << shortest_path(15,35).distance << '\n';
	cout << "Shortest path status from 15 to 35: " << shortest_path(15,35).status << '\n';
	cout << "Shortest path from 15 to 15: " << shortest_path(15,15).distance << '\n';
	cout << "Shortest path status from 15 to 15: " << shortest_path(15,15).status << '\n';
	cout << "Shortest path from 15 to 5: " << shortest_path(15,5).distance << '\n';
	cout << "Shortest path status from 15 to 5: " << shortest_path(15,5).status << '\n';
	cout << "Shortest path from 1 to 5: " << shortest_path(1,5).distance << '\n';
	cout << "Shortest path status from 1 to 5: " << shortest_path(1,5).status << '\n';
	cout << "Shortest path from 1 to 5: " << shortest_path(5,1).distance << '\n';
	cout << "Shortest path status from 1 to 5: " << shortest_path(5,1).status << '\n';
	print_nodes();
	print_graph();
	return 0;
	cout << get_neighbors(20).status << '\n';
	for(uint64_t a : get_neighbors(20).neighbors) {
		cout << "neighbor: " << a << '\n';
	}
	cout << get_neighbors(5).status << '\n';
	for(uint64_t a : get_neighbors(5).neighbors) {
		cout << "neighbor: " << a << '\n';
	}
	cout << get_neighbors(1).status << '\n';
	for(uint64_t a : get_neighbors(1).neighbors) {
		cout << "neighbor: " << a << '\n';
	}


	cout << add_node(1) << '\n';
	cout << add_node(2) << '\n';
	cout << add_node(3) << '\n';
	cout << add_node(4) << '\n';
	cout << add_node(5) << '\n';
	cout << add_node(6) << '\n';
	cout << add_node(7) << '\n';
	cout << add_node(8) << '\n';
	cout << add_node(9) << '\n';
	cout << add_node(0) << '\n';
	cout << add_edge(1,2) << '\n';
	cout << add_edge(1,3) << '\n';
	cout << add_edge(1,4) << '\n';
	cout << add_edge(1,6) << '\n';
	cout << add_edge(1,7) << '\n';
	cout << add_edge(1,8) << '\n';
	cout << add_edge(1,9) << '\n';
	cout << add_edge(3,5) << '\n';
	cout << add_edge(0,5) << '\n';
	return 0;
}
