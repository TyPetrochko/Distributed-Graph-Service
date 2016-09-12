struct returnedNode {
	bool in_graph;
	int status;
};

int add_node(u64 node_id);

int add_edge(u64 node_a_id, u64 node_b_id);

int remove_node(u64 node_id);

int remove_edge(u64 node_a_id, u64 node_b_id);

int get_node(u64 node_id);

int get_edge(u64 node_a_id, u64 node_b_id);

int get_neighbors(u64 node_id);

int shortest_path(u64 node_a_id, uint node_b_id);