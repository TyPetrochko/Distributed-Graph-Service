void replica_init();
void partitioning_init(int partition, std::list<char*> partitions);

// handle partitioning and return success
bool part_add_node(int64_t node_id);
bool part_add_edge(int64_t node_a_id, int64_t node_b_id);
bool part_remove_node(int64_t node_id);
bool part_remove_edge(int64_t node_a_id, int64_t node_b_id);

