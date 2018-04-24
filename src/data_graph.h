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

#define INIT_ARRAY_SIZE 100000

struct node;
struct edge;
struct graph;

typedef struct graph graph;
typedef struct node node;
typedef struct edge edge;

struct node {
	unsigned int index;
	unsigned int indeg;
	unsigned int outdeg;
	edge* firstout;
	edge* lastout;
	edge* firstin;
	edge* lastin;
	node* prev;
	node* next;
};

struct edge {
	unsigned int timestamp;
	int weight;
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
};

void print_nodes(graph *g);

void print_edges(graph *g);

node* exist_node(graph *g, unsigned int node_id);

edge* exist_edge(graph* g, unsigned int timestamp, node* src, node* dst,
									int weight);

void remove_edge (edge* edge_to_remove, graph* g);

void remove_node (node* node_to_remove, graph* g);

node* get_free_node(graph* g);

edge* get_free_edge(graph* g);

int parse_graph(char *file);

void increase_arrays(graph* g);

node* add_source(char *buff, graph *g);

node* add_destination(char* buff, graph* g);

edge* add_edge(char* buff, graph* g, node* source, node* destination);

#endif

