void replica_init();
void master_init(char *ip_adr);

// handle replication if we are replicating
bool repl_add_node(int64_t node_id);
bool repl_add_edge(int64_t node_a_id, int64_t node_b_id);
bool repl_remove_node(int64_t node_id);
bool repl_remove_edge(int64_t node_a_id, int64_t node_b_id);

