#include <cstdint>

struct returnedNode {
	bool in_graph;
	int status;
};

int add_node(uint64_t node_id);

int add_edge(uint64_t node_a_id, uint64_t node_b_id);

int remove_node(uint64_t node_id);

int remove_edge(uint64_t node_a_id, uint64_t node_b_id);

struct nodeData get_node(uint64_t node_id);

struct nodeData get_edge(uint64_t node_a_id, uint64_t node_b_id);

struct neighborData get_neighbors(uint64_t node_id);

int shortest_path(uint64_t node_a_id, uint node_b_id);