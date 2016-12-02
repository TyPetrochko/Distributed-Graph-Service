#include <cstdint>
#include <list>
#include <mutex>

using std::list;

struct nodeData {
	bool in_graph;
	int status;
};

struct neighborData{
	list<int64_t> neighbors;
	int status;
};

struct distanceData {
	int64_t distance;
	int status;
};

int add_node(int64_t node_id);

int add_edge(int64_t node_a_id, int64_t node_b_id);

int remove_node(int64_t node_id);

int remove_edge(int64_t node_a_id, int64_t node_b_id);

struct nodeData get_node(int64_t node_id);

struct nodeData get_edge(int64_t node_a_id, int64_t node_b_id);

struct neighborData get_neighbors(int64_t node_id);

struct distanceData shortest_path(int64_t node_a_id, int64_t node_b_id);

