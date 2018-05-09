/**
This module contrains the code about graph managment.
**/

#ifndef __GRAPH__
#define __GRAPH__

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "utils.h"

#define INIT_ARRAY_SIZE 10000

struct node;
struct edge;
struct graph;

typedef struct graph graph;
typedef struct node node;
typedef struct edge edge;

struct node {
	size_t index;
	size_t indeg;
	size_t outdeg;
	size_t index_array;
	edge* firstout;
	edge* lastout;
	edge* firstin;
	edge* lastin;
	node* prev;
	node* next;
};

struct edge {
	int weight;
	size_t timestamp;
	size_t index_array;
	node* source;
	node* destination;
	edge* sourceprev;
	edge* sourcenext;
	edge* targetprev;
	edge* targetnext;
	edge* prev;
	edge* next;
};

struct graph {
	node* node_array;
	edge* edge_array;
	node* last_node;
	edge* last_edge;
	node* first_free_node;
	edge* first_free_edge;
	size_t nb_edges;
	size_t nb_nodes;
	size_t max_node_index;
	size_t max_edge_index;
	int** dist;
};

void print_node     (node* node);
void print_edge     (edge* edge);
void print_nodes      (graph* g);
void print_edges      (graph* g);
void print_free_edges (graph* g);
void print_free_nodes (graph* g);
void print_node_array (graph* g);
void print_edge_array (graph* g);
void print_nodes_v    (graph* g);
void print_edges_v    (graph* g);
void print_graph      (graph* g);

graph* init_graph(void);
void increase_arrays(graph* g);

node* exist_node(graph* g, size_t node_id);
edge* exist_edge(graph* g, size_t timestamp, node* src, node* dst, int weight);

void remove_edge (edge* edge_to_remove, graph* g);
void remove_node (node* node_to_remove, graph* g);

node* get_free_node(graph* g);
edge* get_free_edge(graph* g);

node* add_source      (char* buff, graph* g);
node* add_destination (char* buff, graph* g);
edge* add_edge        (char* buff, graph* g, node* source, node* destination);

Error_enum parse_graph(char *file);

#endif

