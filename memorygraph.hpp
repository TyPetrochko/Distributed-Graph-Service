#include <cstdint>
#include <list>

using std::list;

struct nodeData {
	bool in_graph;
	int status;
};

struct neighborData{
	list<uint64_t> neighbors;
	int status;
};

struct distanceData {
	uint64_t distance;
	int status;
};

int add_node(uint64_t node_id);

int add_edge(uint64_t node_a_id, uint64_t node_b_id);

int remove_node(uint64_t node_id);

int remove_edge(uint64_t node_a_id, uint64_t node_b_id);

struct nodeData get_node(uint64_t node_id);

struct nodeData get_edge(uint64_t node_a_id, uint64_t node_b_id);

struct neighborData get_neighbors(uint64_t node_id);

struct distanceData shortest_path(uint64_t node_a_id, uint64_t node_b_id);

