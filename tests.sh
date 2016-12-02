#!/bin/bash

# include '--verbose' to include output
curl -H "Content-Type: application/json" -X POST -d '{"node_id":201}' http://130.211.164.50:1111/api/v1/add_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id":202}' http://130.211.164.50:1111/api/v1/add_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id":203}' http://130.211.164.50:1111/api/v1/add_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id":204}' http://130.211.164.50:1111/api/v1/add_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id":205}' http://130.211.164.50:1111/api/v1/add_node

curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":201, "node_b_id":202}' http://130.211.164.50:1111/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":202, "node_b_id":203}' http://130.211.164.50:1111/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":203, "node_b_id":204}' http://130.211.164.50:1111/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":202, "node_b_id":204}' http://130.211.164.50:1111/api/v1/add_edge

curl -H "Content-Type: application/json" -X POST -d '{"node_id":205}' http://130.211.164.50:1111/api/v1/remove_node

curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":201, "node_b_id":202}' http://130.211.164.50:1111/api/v1/remove_edge

curl -H "Content-Type: application/json" -X POST -d '{"node_id":202}' http://130.211.164.50:1111/api/v1/get_neighbors

