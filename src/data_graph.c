/**
This module contains the code about graph managment.
**/

/* TODO:
 *
 * Hash table map index - pointer node and edge for exist_edge and exist_node.
 *
 * Catch errors.
 * Error_enum tmp
 *
 * Removing an edge may disconnect the graph.
 * A timestamp must be strictly positive.
 * An index must be strictly positive.
 *
 * clean tmpedge and new_edge in add_edge.
 *
 */

#include "data_graph.h"

#define INIT_ARRAY_SIZE 100000

void print_nodes(graph *g) {
	printf("Printing current node array.\n");
	node* tmp = g->last_node->next;
	do {
		printf("Node %u, indeg %u, outdeg %u.\n",
						tmp->index, tmp->indeg, tmp->outdeg);
		printf("Prev: %u, next: %u.\n",tmp->prev->index, tmp->next->index);
		printf("Last node: %u.\n", g->last_node->index);
		tmp = tmp->next;
	} while (tmp->prev != tmp);
}

void print_edges(graph *g) {
	edge* tmp = g->last_edge->next;
	do {
		printf("Node %u from %u to %u, of weight %i.\n",
						tmp->timestamp, tmp->source->index,
						tmp->destination->index, tmp->weight);
		printf("Prev: (%u,%u), next: (%u,%u).\n",
										tmp->prev->source->index, tmp->prev->destination->index,
										tmp->next->source->index, tmp->next->destination->index);
		tmp = tmp->next;
	} while (tmp->prev != tmp);
}

/* Would be more efficient with a hash table. */
node* exist_node(graph *g, unsigned int node_id) {
	/* Prevents segfault on first node. */
	if (g->last_node != NULL) {
		node* tmp = g->last_node->next;
		do {
			printf("Looking for %u. This is %u.\n", node_id, tmp->index);
			if (tmp->index == node_id)
				return tmp;
			tmp = tmp->next;
		} while (tmp->prev != tmp);
	} else
		printf("This is the first node, skipping search for duplicate.\n");
	return NULL;
}

/* Would be more efficient with a hash table. */
edge* exist_edge(graph* g, unsigned int timestamp, node* src, node* dst,
									int weight)
{
	/* Prevents segfault on first edge. */
	if (g->last_edge != NULL) {
		edge* tmp = g->last_edge->next;
		do {
			printf("Looking for (%u,%u), this is (%u,%u).\n", tmp->source->index,
									tmp->destination->index, src->index, dst->index);
			if (tmp->source == src && tmp->destination == dst) {
				if (tmp->timestamp == timestamp) {
					printf("Found matching (%u,%u) and (%u,%u).\n", tmp->source->index,
									tmp->destination->index, src->index, dst->index);
					return tmp;
				} else {
					printf("Found the edge. Updating the timestamp and weight.\n");
					tmp->timestamp = timestamp;
					tmp->weight = weight;
				}
			}
			tmp = tmp->next;
		} while (tmp->prev != tmp);
	} else {
		printf("This is the first edge, skipping search for duplicate.\n");
	}
	return NULL;
}

void remove_edge (edge* edge_to_remove, graph* g) {
	if (edge_to_remove == g->last_edge) {
		g->last_edge = edge_to_remove->prev;
	}

	/* If it is the first edge, prev is self. */
	if (edge_to_remove == g->last_edge->next) {
		edge_to_remove->next->prev = edge_to_remove->next;
		g->last_edge->next = edge_to_remove->next;
	} else {
		edge_to_remove->next->prev = edge_to_remove->prev;
		edge_to_remove->prev->next = edge_to_remove->next;
	}

	edge_to_remove->next        = g->first_free_edge;
	edge_to_remove->prev        = edge_to_remove;
	edge_to_remove->timestamp   = 0;
	edge_to_remove->weight      = 0;
	edge_to_remove->source      = NULL;
	edge_to_remove->destination = NULL;
	edge_to_remove->sourceprev  = NULL;
	edge_to_remove->sourcenext  = NULL;
	edge_to_remove->targetprev  = NULL;
	edge_to_remove->targetnext  = NULL;

	g->first_free_edge->next = g->first_free_edge;
	g->first_free_edge       = edge_to_remove;
}

void remove_node (node* node_to_remove, graph* g) {
	if (node_to_remove == g->last_node) {
		g->last_node = node_to_remove->prev;
	}

	/* If it is the first node, prev is self. */
	if (node_to_remove == g->last_node->next) {
		node_to_remove->next->prev = node_to_remove->next;
		g->last_node->next = node_to_remove->next;
	} else {
		node_to_remove->next->prev = node_to_remove->prev;
		node_to_remove->prev->next = node_to_remove->next;
	}

	/* Removing outgoing edges. */
	edge *tmp = node_to_remove->firstout;
	while (tmp != NULL){
		printf("Removing unnecessary edge (%u,%u).\n",
						tmp->source->index, tmp->destination->index);
		remove_edge(tmp, g);
		tmp = tmp->sourcenext;
	}

	/* Removing incoming edges. */
	tmp = node_to_remove->firstin;
	while (tmp != NULL){
		printf("Removing unnecessary edge (%u,%u).\n",
						tmp->source->index, tmp->destination->index);
		remove_edge(tmp, g);
		tmp = tmp->targetnext;
	}

	node_to_remove->next     = g->first_free_node;
	node_to_remove->prev     = node_to_remove;
	node_to_remove->index    = 0;
	node_to_remove->indeg    = 0;
	node_to_remove->outdeg   = 0;
	node_to_remove->firstout = NULL;
	node_to_remove->lastout  = NULL;
	node_to_remove->firstin  = NULL;
	node_to_remove->lastin   = NULL;


	g->first_free_node->next = g->first_free_node;
	g->first_free_node       = node_to_remove;
}

node* get_free_node(graph* g) {
	node* res = g->first_free_node;
	if ((g->first_free_node + 1)->index == 0) {
		g->first_free_node++;
	} else
		g->first_free_node = g->first_free_node->next;
	return res;
}

edge* get_free_edge(graph* g) {
	edge* res = g->first_free_edge;
	if ((g->first_free_edge + 1)->timestamp == 0) {
		g->first_free_edge++;
	} else
		g->first_free_edge = g->first_free_edge->next;
	return res;
}

graph* init_graph(void) {
	size_t max_array_node = INIT_ARRAY_SIZE;
	size_t max_array_edge = INIT_ARRAY_SIZE;

	graph *g = malloc_wrapper(sizeof(*g));

	g->node_array = calloc_wrapper(max_array_node, sizeof(node));
	g->edge_array = calloc_wrapper(max_array_edge, sizeof(edge));
	g->last_node  = NULL;
	g->last_edge  = NULL;
	g->first_free_node = g->node_array;
	g->first_free_edge = g->edge_array;

	return g;
}

void increase_arrays(graph* g) {
		size_t max_array_node = INIT_ARRAY_SIZE;
		size_t max_array_edge = INIT_ARRAY_SIZE;

		if (g->edge_array[max_array_edge-1].timestamp != 0)
			g->edge_array = realloc_wrapper(g->edge_array, &max_array_edge,
																			sizeof(g->edge_array));
		if (g->node_array[max_array_node-1].index != 0)
			g->node_array = realloc_wrapper(g->node_array, &max_array_node,
																			sizeof(g->node_array));
}

node* add_source(char *buff, graph *g) {
	printf("The source node is: %s.\n", buff);

	unsigned int index = 0;
	Error_enum tmp = strtoui_wrapper(buff, &index);
	node *source = exist_node(g, index);

	if (!source) {
		printf("This node wasn't in memory.\n");
		node* new_node  = get_free_node(g);
		new_node->index = index;
		printf("Created node: %u.\n", new_node->index);
		source         = new_node;
		source->indeg  = 0;
		source->outdeg = 0;

		/* If this is the first node. */
		if (g->node_array[1].index == 0) {
			source->prev = source;
			source->next = source;
			g->last_node = source;
		} else if (g->node_array[2].index == 0) {
			source->prev = g->last_node;
			source->next = g->last_node;
			g->last_node->next = source;
			g->last_node       = source;
		} else {
			source->next = g->last_node->next;
			source->prev = g->last_node;
			g->last_node->next = source;
			g->last_node       = source;
		}
		printf("Here is the source index: %u.\n", source->index);
		printf("Successfully imported: %s.\n\n", buff);

	} else {
		printf("Here is the source index: %u.\n", source->index);
		printf("It looks like that node was already in memory.\n\n");
	}
	return source;
}

node* add_destination(char* buff, graph* g) {
	buff = strtok(NULL, " ");
	printf("The destination node is: %s.\n", buff);

	unsigned int index = 0;
	Error_enum tmp = strtoui_wrapper(buff, &index);
	node *destination = exist_node(g, index);

	if (!destination) {
		printf("This node wasn't in memory.\n");
		node* new_node  = get_free_node(g);
		new_node->index = index;
		printf("Created node: %u.\n", new_node->index);
		destination         = new_node;
		destination->indeg  = 0;
		destination->outdeg = 0;
		destination->prev   = g->last_node;

		if (g->node_array[1].index == 0) {
			/* Not needed as long as destination is parsed after source. */
			destination->prev = destination;
			destination->next = destination;
			g->last_node      = destination;
		} else if (g->node_array[2].index == 0) {
			destination->prev  = g->last_node;
			destination->next  = g->last_node;
			g->last_node->next = destination;
			g->last_node       = destination;
		} else {
			destination->next  = g->last_node->next;
			destination->prev  = g->last_node;
			g->last_node->next = destination;
			g->last_node       = destination;
		}
		printf("Here is the destination index: %u.\n", destination->index);
		printf("Successfully imported: %s.\n\n", buff);
	} else {
		printf("Here is the destination index: %u.\n", destination->index);
		printf("It looks like that node was already in memory.\n\n");
	}
	return destination;
}

edge* add_edge(char* buff, graph* g, node* source, node* destination) {
	buff = strtok(NULL, " ");
	int weight = 0;
	Error_enum tmp1 = strtoi_wrapper(buff, &weight);

	buff = strtok(NULL, " ");
	unsigned int timestamp = 0;
	Error_enum tmp2 = strtoui_wrapper(buff, &timestamp);

	printf("the edge timestamp is %u.\n",timestamp);

	edge *tmpedge = exist_edge(g, timestamp, source, destination, weight);

	if (!tmpedge) {
		printf("this edge wasn't in memory.\n");
		edge* new_edge        = get_free_edge(g);
		new_edge->timestamp   = timestamp;
		new_edge->weight      = weight;
		new_edge->source      = source;
		new_edge->destination = destination;

		if (g->edge_array[1].timestamp == 0) {
			new_edge->prev = new_edge;
			new_edge->next = new_edge;
			g->last_edge   = new_edge;
		} else if (g->edge_array[2].timestamp == 0) {
			new_edge->prev     = g->last_edge;
			new_edge->next     = g->last_edge;
			g->last_edge->next = new_edge;
			g->last_edge       = new_edge;
		} else {
			new_edge->next     = g->last_edge->next;
			new_edge->prev     = g->last_edge;
			g->last_edge->next = new_edge;
			g->last_edge       = new_edge;
		}

		/* set up or updates pointers. */
		if (new_edge->source->lastout != NULL)
			new_edge->sourceprev = new_edge->source->lastout;
		else
			new_edge->sourceprev = new_edge;

		new_edge->source->lastout        = new_edge;
		new_edge->sourceprev->sourcenext = new_edge;
		if (new_edge->source->firstout != NULL)
			new_edge->source->firstout = new_edge;
		else
			new_edge->source->firstin = new_edge;

		if (new_edge->source->lastin != NULL)
			new_edge->targetprev = new_edge->destination->lastin;
		else
			new_edge->targetprev = new_edge;

		new_edge->destination->lastin    = new_edge;
		new_edge->targetprev->targetnext = new_edge;
		if (new_edge->destination->firstin != NULL)
			new_edge->destination->firstin = new_edge;
		else
			new_edge->destination->firstin = new_edge;

		new_edge->source->outdeg++;
		new_edge->destination->indeg++;

		tmpedge = new_edge;
		printf("successfully imported: %u.\n\n", new_edge->timestamp);
	} else {
		printf("it looks like that edge was already in memory.\n\n");
	}
	return tmpedge;
}

int parse_graph(char *file) {
	FILE *f = fopen_wrapper(file, "r");

	char buffer[BUFSIZ];

	graph* g = init_graph();

	/* Reads line after line. */
	while (fgets(buffer, BUFSIZ, f)) {
		increase_arrays(g);

		/* Format error or comment. */
		char *buff = strtok(buffer, " ");
		if (!buff)
			return FILE_FORMAT_ERROR;
		else if (*buff == '%') {
			printf("This is a comment.\n\n");
		} else {

			/* Parsing the nodes. */
			node* source      = add_source(buff, g);
			node* destination = add_destination(buff, g);

			/* Parsing the edge. */
			edge* new_edge = add_edge(buff, g, source, destination);
		}
	}

	print_nodes(g);
	print_edges(g);
	fclose(f);

	/* TESTS. */
	remove_node(g->last_node, g);
	printf("Removed the last node.\n");
	print_nodes(g);
	print_edges(g);
	remove_edge(g->last_edge, g);
	printf("Removed the last edge.\n");
	print_nodes(g);
	print_edges(g);

	return NO_ERROR;
}

