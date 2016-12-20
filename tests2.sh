#!/bin/bash

# include '--verbose' to include output

echo "Making nodes"
curl -H "Content-Type: application/json" -X POST -d '{"node_id":200}' http://127.0.0.1:9000/api/v1/add_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id":202}' http://127.0.0.1:9000/api/v1/add_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id":201}' http://127.0.0.1:9001/api/v1/add_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id":203}' http://127.0.0.1:9001/api/v1/add_node

read -n1 -r -p "Press any key to continue..." key

echo "Testing get_node"

curl -H "Content-Type: application/json" -X POST -d '{"node_id":201}' http://127.0.0.1:9001/api/v1/get_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id":202}' http://127.0.0.1:9000/api/v1/get_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id":203}' http://127.0.0.1:9001/api/v1/get_node

read -n1 -r -p "Press any key to continue..." key

echo "Testing add_edge"

curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":200, "node_b_id":202}' http://127.0.0.1:9000/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":200, "node_b_id":201}' http://127.0.0.1:9000/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":200, "node_b_id":203}' http://127.0.0.1:9000/api/v1/add_edge

read -n1 -r -p "Press any key to continue..." key

echo "Testing get_neighbors"

curl -H "Content-Type: application/json" -X POST -d '{"node_id":200}' http://127.0.0.1:9000/api/v1/get_neighbors
read -n1 -r -p "Press any key to continue..." key
curl -H "Content-Type: application/json" -X POST -d '{"node_id":201}' http://127.0.0.1:9001/api/v1/get_neighbors
read -n1 -r -p "Press any key to continue..." key
curl -H "Content-Type: application/json" -X POST -d '{"node_id":202}' http://127.0.0.1:9000/api/v1/get_neighbors
read -n1 -r -p "Press any key to continue..." key
curl -H "Content-Type: application/json" -X POST -d '{"node_id":203}' http://127.0.0.1:9001/api/v1/get_neighbors

# curl -H "Content-Type: application/json" -X POST -d '{"node_id":205}' http://127.0.0.1:9000/api/v1/remove_node
# 
# curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":201, "node_b_id":202}' http://127.0.0.1:9000/api/v1/remove_edge
# 
# curl -H "Content-Type: application/json" -X POST -d '{"node_id":202}' http://127.0.0.1:9000/api/v1/get_neighbors

