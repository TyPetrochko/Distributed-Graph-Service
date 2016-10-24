#!/bin/bash

curl -H "Content-Type: application/json" -X POST -d '{"node_id":1}' http://127.0.0.1:8000/api/v1/add_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id":2}' http://127.0.0.1:8000/api/v1/add_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id":3}' http://127.0.0.1:8000/api/v1/add_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id":4}' http://127.0.0.1:8000/api/v1/add_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id":5}' http://127.0.0.1:8000/api/v1/add_node

curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":1, "node_b_id":2}' http://127.0.0.1:8000/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":2, "node_b_id":3}' http://127.0.0.1:8000/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":3, "node_b_id":4}' http://127.0.0.1:8000/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":2, "node_b_id":4}' http://127.0.0.1:8000/api/v1/add_edge

curl -H "Content-Type: application/json" -X POST -d '{"node_id":5}' http://127.0.0.1:8000/api/v1/remove_node

curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":1, "node_b_id":2}' http://127.0.0.1:8000/api/v1/remove_edge

curl -H "Content-Type: application/json" -X POST -d '{"node_id":2}' http://127.0.0.1:8000/api/v1/get_neighbors

