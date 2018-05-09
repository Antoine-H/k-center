/*
 * This module contains the code about graph managment.
 * Removing an edge may disconnect the graph.
 */

/* TODO:
 *
 * Hash table map index - pointer node and edge for exist_edge and exist_node.
 * Need a map node index - array index. Edge index - array index.
 *
 * Catch errors from strto.
 *
 * Redo update dist.
 */

#include "data_graph.h"
#include "dynamic_all_pairs_shortest_paths.h"

size_t max_array_edge = INIT_ARRAY_SIZE;
size_t max_array_node = INIT_ARRAY_SIZE;

void print_nodes(graph *g) {
	if (g->nb_nodes > 0) {
		node* tmp = g->last_node->next;
		printf("Printing current node array.\n");
		do {
			printf("Node %zu, indeg %zu, outdeg %zu, ",
							tmp->index, tmp->indeg, tmp->outdeg);
			printf("prev %zu, next %zu, ",tmp->prev->index, tmp->next->index);
			printf("last node %zu.\n", g->last_node->index);
			tmp = tmp->next;
		} while (tmp->prev != tmp);
	} else {
		printf("There are no nodes yet.\n");
	}
}

void print_edges(graph *g) {
	if (g->nb_edges > 0) {
		edge* tmp = g->last_edge->next;
		printf("Printing current edges array.\n");
		do {
			printf("Edge %zu from %zu to %zu, of weight %i, ",
							tmp->timestamp, tmp->source->index,
							tmp->destination->index, tmp->weight);
			printf("prev (%zu,%zu), next (%zu,%zu).\n",
											tmp->prev->source->index, tmp->prev->destination->index,
											tmp->next->source->index, tmp->next->destination->index);
			tmp = tmp->next;
		} while (tmp->prev != tmp);
	} else {
		printf("There are no edges yet.\n");
	}
}

void print_free_nodes (graph* g) {
	node *tmp = g->first_free_node;
	printf("Printing current free nodes array.\n");
	printf("Index %zu.\n", tmp->index);
	while (tmp->next != tmp) {
		tmp = tmp->next;
		printf("Index %zu.\n", tmp->index);
	}
}

void print_free_edges (graph* g) {
	edge *tmp = g->first_free_edge;
	printf("Printing current free edges array.\n");
	printf("Timestamp %zu.\n", tmp->timestamp);
	while (tmp->next != tmp) {
		tmp = tmp->next;
		printf("Timestamp %zu.\n", tmp->timestamp);
	}
}

void print_node(node* node) {
	printf("Index:      %zu.\n", node->index);
	printf("In degree:  %zu.\n", node->indeg);
	printf("Out degree: %zu.\n", node->outdeg);
	if (node->firstin != NULL)
		printf("Firstin:    %zu.\n", node->firstin->timestamp);
	else
		printf("firstin:    NULL.\n");
	if (node->firstin != NULL)
		printf("Lastin:     %zu.\n", node->lastin->timestamp);
	else
		printf("Lastin:     NULL.\n");
	if (node->firstout != NULL)
		printf("Firstout:   %zu.\n", node->firstout->timestamp);
	else
		printf("Firstout:   NULL.\n");
	if (node->lastout != NULL)
		printf("Lasttout:   %zu.\n", node->lastout->timestamp);
	else
		printf("Lasttout:   NULL.\n");
	if (node->prev != NULL)
		printf("Previous:   %zu.\n", node->prev->index);
	else
		printf("Previous:   NULL.\n");
	if (node->next != NULL)
		printf("Next:       %zu.\n", node->next->index);
	else
		printf("Next:       NULL.\n");
	printf("\n");
}

void print_edge (edge* edge) {
	printf("Timestamp:   %zu.\n", edge->timestamp);
	printf("Weight:      %u.\n", edge->weight);
	if (edge->source != NULL)
		printf("Source:      %zu.\n", edge->source->index);
	else
		printf("Source:      NULL.\n");
	if (edge->destination != NULL)
		printf("Destination: %zu.\n", edge->destination->index);
	else
		printf("Destination: NULL.\n");
	if (edge->sourceprev != NULL)
		printf("Sourceprev:  %zu.\n", edge->sourceprev->timestamp);
	else
		printf("Sourceprev:  NULL.\n");
	if (edge->sourcenext != NULL)
		printf("Sourcenext:  %zu.\n", edge->sourcenext->timestamp);
	else
		printf("Sourcenext:  NULL.\n");
	if (edge->targetprev != NULL)
		printf("Targetprev:  %zu.\n", edge->targetprev->timestamp);
	else
		printf("Targetprev:  NULL.\n");
	if (edge->targetnext != NULL)
		printf("Targetnext:  %zu.\n", edge->targetnext->timestamp);
	else
		printf("Targetnext:  NULL.\n");
	if (edge->prev != NULL)
		printf("Prev:        %zu.\n", edge->prev->timestamp);
	else
		printf("Prev:        NULL.\n");
	if (edge->next != NULL)
		printf("Next:        %zu.\n", edge->next->timestamp);
	else
		printf("Next:        NULL.\n");
	printf("\n");
}

void print_graph(graph* g) {
	printf("Last node:\n");
	print_node(g->last_node);
	printf("Last edge:\n");
	print_edge(g->last_edge);
	printf("First free node:\n");
	print_node(g->first_free_node);
	printf("First free edge:\n");
	print_edge(g->first_free_edge);
	printf("Nb nodes: %zu.\n", g->nb_nodes);
	printf("Nb edges: %zu.\n", g->nb_edges);
}

void print_node_array (graph* g) {
	size_t i = 0;
	for (i = 0; i < g->max_node_index; i++) {
		printf("%zu - %zu.\n", i, g->node_array[i].index);
		if (&g->node_array[i] == g->first_free_node)
			printf("First free node is right above.\n");
	}
	printf("----------------\n");
}

void print_edge_array (graph* g) {
	size_t i = 0;
	for (i = 0; i < g->max_edge_index; i++) {
		printf("%zu - %zu.\n", i, g->edge_array[i].timestamp);
		if (&g->edge_array[i] == g->first_free_edge)
			printf("Frist free edge is right above.\n");
	}
	printf("----------------\n");
}

void print_nodes_v (graph* g){
	node* tmp = g->last_node->next;
	do {
		print_node(tmp);
		tmp = tmp->next;
	} while (tmp->prev != tmp);
}

void print_edges_v (graph *g) {
	edge* tmp = g->last_edge->next;
	do {
		print_edge(tmp);
		tmp = tmp->next;
	} while (tmp->prev != tmp);
}

/* Would be more efficient with a hash table. */
node* exist_node(graph* g, size_t node_id) {
	/* Prevents segfault on first node. */
	if (g->nb_nodes != 0) {
		node* tmp = g->last_node->next;
		do {
			/*printf("Looking for %zu. This is %zu.\n", node_id, tmp->index);*/
			if (tmp->index == node_id)
				return tmp;
			tmp = tmp->next;
		} while (tmp->prev != tmp);
	} else
		printf("This is the first node, skipping search for duplicate.\n");
	return NULL;
}

/* Would be more efficient with a hash table. */
edge* exist_edge(graph* g, size_t timestamp, node* src, node* dst, int weight)
{
	/* Prevents segfault on first edge. */
	if (g->nb_edges != 0) {
		edge* tmp = g->last_edge->next;
		do {
			/*printf("Looking for (%zu,%zu), this is (%zu,%zu).\n", src->index,
							dst->index, tmp->source->index, tmp->destination->index);*/
			if (tmp->source == src && tmp->destination == dst) {
				/* Adding an edge with the same source, same destination and same
				* timestamp as one already in memory is considered an error in the
				* input file. */
				if (tmp->timestamp == timestamp) {
					printf("Found matching (%zu,%zu) and (%zu,%zu).\n",
									tmp->source->index, tmp->destination->index,
									src->index, dst->index);
					return tmp;
				} else {
					printf("Found the edge. Updating the timestamp and weight.\n");
					tmp->timestamp = timestamp;
					tmp->weight = weight;
					return tmp;
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

	if (edge_to_remove->destination->firstin == edge_to_remove &&
			edge_to_remove->destination->lastin  == edge_to_remove) {
		edge_to_remove->destination->firstin = NULL;
		edge_to_remove->destination->lastin  = NULL;
	} else if (edge_to_remove->destination->firstin == edge_to_remove) {
		edge_to_remove->targetnext->targetprev = edge_to_remove->targetnext;
		edge_to_remove->destination->firstin = edge_to_remove->targetnext;
	} else if (edge_to_remove->destination->lastin  == edge_to_remove) {
		edge_to_remove->targetprev->targetnext = edge_to_remove->targetnext;
		edge_to_remove->destination->lastin  = edge_to_remove->targetprev;
	} else {
		edge_to_remove->targetnext->targetprev = edge_to_remove->targetprev;
		edge_to_remove->targetprev->targetnext = edge_to_remove->targetnext;
	}
	edge_to_remove->targetprev = NULL;
	edge_to_remove->targetnext = NULL;

	if (edge_to_remove->source->firstout == edge_to_remove &&
			edge_to_remove->source->lastout  == edge_to_remove) {
		edge_to_remove->source->firstout = NULL;
		edge_to_remove->source->lastout  = NULL;
	} else if (edge_to_remove->source->firstout == edge_to_remove) {
		edge_to_remove->sourcenext->sourceprev = edge_to_remove->sourcenext;
		edge_to_remove->source->firstout = edge_to_remove->sourcenext;
	} else if (edge_to_remove->source->lastout == edge_to_remove) {
		edge_to_remove->sourceprev->sourcenext = edge_to_remove->sourceprev;
		edge_to_remove->source->lastout  = edge_to_remove->sourceprev;
	} else {
		edge_to_remove->sourcenext->sourceprev = edge_to_remove->sourceprev;
		edge_to_remove->sourceprev->sourcenext = edge_to_remove->sourcenext;
	}
	edge_to_remove->sourceprev = NULL;
	edge_to_remove->sourcenext = NULL;

	edge_to_remove->source->outdeg--;
	edge_to_remove->destination->indeg--;

	if (g->first_free_edge == &g->edge_array[g->max_edge_index]) {
		g->first_free_edge = edge_to_remove;
		g->first_free_edge->prev = edge_to_remove;
		g->first_free_edge->next = g->last_edge + 1;
	} else {
		g->first_free_edge->prev = edge_to_remove;
		edge_to_remove->next = g->first_free_edge;
		g->first_free_edge = edge_to_remove;
		g->first_free_edge->prev = edge_to_remove;
	}

	edge_to_remove->timestamp   = 0;
	edge_to_remove->weight      = 0;
	edge_to_remove->source      = NULL;
	edge_to_remove->destination = NULL;
	edge_to_remove->sourceprev  = NULL;
	edge_to_remove->sourcenext  = NULL;
	edge_to_remove->targetprev  = NULL;
	edge_to_remove->targetnext  = NULL;

	g->nb_edges--;
}

void remove_node (node* node_to_remove, graph* g) {
	edge *tmp = NULL;

	if (node_to_remove == g->last_node) {
		g->last_node = node_to_remove->prev;
	}

	/* Removing outgoing edges. */
	tmp = node_to_remove->firstout;
	while (node_to_remove->outdeg != 0) {
		/*printf("Removing unnecessary outgoing edge (%zu,%zu).\n",
						tmp->source->index, tmp->destination->index);*/
		tmp = tmp->sourcenext;
		remove_edge(tmp->sourceprev, g);
	}

	/* Removing incoming edges. */
	tmp = node_to_remove->firstin;
	while (node_to_remove->indeg != 0) {
		/*printf("Removing unnecessary incoming edge (%zu,%zu).\n",
						tmp->source->index, tmp->destination->index);*/
		tmp = tmp->targetnext;
		remove_edge(tmp->targetprev, g);
	}

	/* If it is the first node, prev is self. */
	if (node_to_remove == g->last_node->next) {
		node_to_remove->next->prev = node_to_remove->next;
		g->last_node->next = node_to_remove->next;
	} else {
		node_to_remove->next->prev = node_to_remove->prev;
		node_to_remove->prev->next = node_to_remove->next;
	}

	if (g->first_free_node == &g->node_array[g->nb_nodes]) {
		g->first_free_node = node_to_remove;
		g->first_free_node->prev = node_to_remove;
		g->first_free_node->next = g->last_node + 1;
	} else {
		g->first_free_node->prev = node_to_remove;
		node_to_remove->next = g->first_free_node;
		g->first_free_node = node_to_remove;
		g->first_free_node->prev = node_to_remove;
	}

	node_to_remove->index    = 0;
	node_to_remove->indeg    = 0;
	node_to_remove->outdeg   = 0;
	node_to_remove->firstout = NULL;
	node_to_remove->lastout  = NULL;
	node_to_remove->firstin  = NULL;
	node_to_remove->lastin   = NULL;

	g->nb_nodes--;

}

node* get_free_node(graph* g) {
	node* res = g->first_free_node;
	/*print_node_array(g);
	printf("%zu.\n", g->nb_nodes);*/
	if (g->first_free_node == &g->node_array[g->max_node_index]) {
		printf("Incrementing first_free_node.\n");
		g->first_free_node->index_array = g->nb_nodes;
		g->first_free_node++;
		g->first_free_node->prev = g->first_free_node;
		g->first_free_node->next = g->first_free_node;
	} else {
		printf("Using first free node pointer.\n");
		g->first_free_node->index_array = g->first_free_node->prev->index_array;
		g->first_free_node->prev = NULL;
		g->first_free_node = g->first_free_node->next;
		g->first_free_node->prev->next = NULL;
		g->first_free_node->prev = g->first_free_node;
	}
	return res;
}

edge* get_free_edge(graph* g) {
	/*print_edge_array(g);
	printf("%zu.\n", g->nb_edges);*/
	edge* res = g->first_free_edge;
	if (g->first_free_edge == &g->edge_array[g->max_edge_index]) {
		printf("Incrementing first_free_edge.\n");
		g->first_free_edge++;
		g->first_free_edge->prev = g->first_free_edge;
		g->first_free_edge->next = g->first_free_edge;
	} else {
		printf("Using first free edge pointer.\n");
		g->first_free_edge->prev = NULL;
		g->first_free_edge = g->first_free_edge->next;
		g->first_free_edge->prev->next = NULL;
		g->first_free_edge->prev = g->first_free_edge;
	}
	return res;
}

void done_graph(graph *g) {
	unsigned int m;
	free(g->node_array);
	free(g->edge_array);

	for (m = 0; m < max_array_node; m++)
		free(g->dist[m]);

	free(g->dist);
	free(g);
}

graph* init_graph(void) {
	unsigned int i, j;
	graph *g = malloc_wrapper(sizeof(graph));

	g->node_array = calloc_wrapper(max_array_node, sizeof(node));
	g->edge_array = calloc_wrapper(max_array_edge, sizeof(edge));
	g->last_node  = g->node_array;
	g->last_edge  = g->edge_array;
	g->first_free_node = g->node_array;
	g->first_free_edge = g->edge_array;
	g->nb_nodes = 0;
	g->nb_edges = 0;
	g->max_edge_index = 0;
	g->max_node_index = 0;

	g->first_free_edge->prev = g->first_free_edge;
	g->first_free_edge->next = g->first_free_edge;
	g->first_free_node->next = g->first_free_node;
	g->first_free_node->next = g->first_free_node;

	g->dist = calloc_wrapper(max_array_node, sizeof(int*));
	for (i = 0; i < max_array_node; i++)
		g->dist[i] = calloc_wrapper(max_array_node, sizeof(int));

	/* Temporary init to 100. */
	for (i = 0; i < max_array_node; i++) {
		for (j = 0; j < max_array_node; j++) {
			g->dist[i][j] = 100;
		}
	}
	return g;
}

void increase_arrays(graph* g) {
		if (g->edge_array[max_array_edge-1].timestamp != 0) {
			print_edge_array(g);
			g->edge_array = realloc_wrapper(g->edge_array, &max_array_edge,
																			sizeof(edge));
			g->last_edge = g->edge_array + (g->nb_edges - 1);
			g->first_free_edge = g->edge_array + g->nb_edges;
		} if (g->node_array[max_array_node-1].index != 0) {
			print_node_array(g);
			g->node_array = realloc_wrapper(g->node_array, &max_array_node,
																			sizeof(node));
			print_node_array(g);
			g->last_node = g->node_array + (g->nb_nodes - 1);
			g->first_free_node = g->node_array + g->nb_nodes;
		}
}

node* add_source(char* buff, graph *g) {
	unsigned int index = 0;
	Error_enum tmp     = 0;
	node *source       = NULL;

	if ((tmp = strtoui_wrapper(buff, &index))) {
		printf("There has been an error importing the file. Source node.\n");
		exit(EXIT_FAILURE);
	}

	source = exist_node(g, index);

	printf("The source node is: %s.\n", buff);

	if (!source) {
		printf("This node wasn't in memory.\n");
		source         = get_free_node(g);
		source->index  = index;
		printf("Created node: %zu.\n", source->index);
		source->indeg  = 0;
		source->outdeg = 0;

		/* If this is the first node. */
		if (g->nb_nodes == 0) {
			source->prev = source;
			source->next = source;
		} else if (g->nb_nodes == 1) {
			source->prev = g->last_node;
			source->next = g->last_node;
			g->last_node->next = source;
		} else {
			source->next = g->last_node->next;
			source->prev = g->last_node;
			g->last_node->next = source;
		}
		g->last_node       = source;

		if (g->nb_nodes == g->max_node_index)
			g->max_node_index++;
		g->nb_nodes++;

		printf("Here is the source index: %zu.\n", source->index);
		printf("Successfully imported: %s.\n\n", buff);

	} else {
		printf("Here is the source index: %zu.\n", source->index);
		printf("It looks like that node was already in memory.\n\n");
	}
	return source;
}

node* add_destination(char* buff, graph* g) {
	unsigned int index = 0;
	Error_enum tmp     = 0;
	node *destination  = NULL;

	buff = strtok(NULL, " ");
	if ((tmp  = strtoui_wrapper(buff, &index))) {
		printf("There has been an error importing the file. Destination node.\n");
		exit(EXIT_FAILURE);
	}
	destination = exist_node(g, index);

	printf("The destination node is: %s.\n", buff);

	if (!destination) {
		printf("This node wasn't in memory.\n");
		destination         = get_free_node(g);
		destination->index  = index;
		printf("Created node: %zu.\n", destination->index);
		destination->indeg  = 0;
		destination->outdeg = 0;
		destination->prev   = g->last_node;

		if (g->nb_nodes == 0) {
			/* Not needed as long as destination is parsed after source. */
			destination->prev = destination;
			destination->next = destination;
		} else if (g->nb_nodes == 1) {
			destination->prev  = g->last_node;
			destination->next  = g->last_node;
			g->last_node->next = destination;
		} else {
			destination->next  = g->last_node->next;
			destination->prev  = g->last_node;
			g->last_node->next = destination;
		}
		g->last_node       = destination;

		if (g->nb_nodes == g->max_node_index)
			g->max_node_index++;
		g->nb_nodes++;

		printf("Here is the destination index: %zu.\n", destination->index);
		printf("Successfully imported: %s.\n\n", buff);
	} else {
		printf("Here is the destination index: %zu.\n", destination->index);
		printf("It looks like that node was already in memory.\n\n");
	}
	return destination;
}

edge* add_edge(char* buff, graph* g, node* source, node* destination) {
	int weight       = 0;
	unsigned int timestamp = 0;
	Error_enum tmp   = 0;
	edge *new_edge   = NULL;

	buff = strtok(NULL, " ");
	if ((tmp  = strtoi_wrapper(buff, &weight))) {
		printf("There has been an error importing the file. Weight\n");
		exit(EXIT_FAILURE);
	}

	buff = strtok(NULL, " ");
	if ((tmp  = strtoui_wrapper(buff, &timestamp))) {
		/*printf("There has been an error importing the file. Timestamp.\n");*/
		/*exit(EXIT_FAILURE);*/
	}

	new_edge = exist_edge(g, timestamp, source, destination, weight);

	printf("The edge timestamp is  %u.\n",timestamp);

	if (!new_edge) {
		printf("This edge wasn't in memory.\n");
		new_edge              = get_free_edge(g);
		new_edge->timestamp   = timestamp;
		new_edge->weight      = weight;
		new_edge->source      = source;
		new_edge->destination = destination;

		if (g->nb_edges == 0) {
			new_edge->prev = new_edge;
			new_edge->next = new_edge;
		} else if (g->nb_edges == 1) {
			new_edge->prev     = g->last_edge;
			new_edge->next     = g->last_edge;
			g->last_edge->next = new_edge;
		} else {
			new_edge->next     = g->last_edge->next;
			new_edge->prev     = g->last_edge;
			g->last_edge->next = new_edge;
		}
		g->last_edge       = new_edge;

		/* Sets up or updates pointers. */
		if (new_edge->source->outdeg == 0)
			new_edge->sourceprev = new_edge;
		else
			new_edge->sourceprev = new_edge->source->lastout;

		new_edge->sourceprev->sourcenext = new_edge;
		new_edge->sourcenext             = new_edge;
		new_edge->source->lastout        = new_edge;

		if (new_edge->source->outdeg == 0)
			new_edge->source->firstout = new_edge;

		if (new_edge->destination->indeg == 0)
			new_edge->targetprev = new_edge;
		else
			new_edge->targetprev = new_edge->destination->lastin;

		new_edge->targetprev->targetnext = new_edge;
		new_edge->destination->lastin    = new_edge;
		new_edge->targetnext             = new_edge;

		if (new_edge->destination->indeg == 0)
			new_edge->destination->firstin = new_edge;

		new_edge->source->outdeg++;
		new_edge->destination->indeg++;

		if (g->nb_edges == g->max_edge_index)
			g->max_edge_index++;
		g->nb_edges++;

		printf("Successfully imported: %zu.\n\n", new_edge->timestamp);
	} else {
		printf("It looks like that edge was already in memory.\n\n");
	}
	return new_edge;
}

Error_enum parse_graph(char* file) {

	FILE  *f              = fopen_wrapper(file, "r");
	char   buffer[BUFSIZ] = "";
	char  *buff           = "";
	graph *g              = init_graph();

	/* Reads line after line. */
	while (fgets(buffer, BUFSIZ, f)) {
		increase_arrays(g);
		buff = strtok(buffer, " ");
		if (!buff)
			return FILE_FORMAT_ERROR;
		else if (*buff == '%') {
			printf("This is a comment.\n\n");
		} else {
			node *source           = add_source(buff,g);
			node *destination = add_destination(buff,g);
			add_edge(buff, g, source, destination);
			update_dist(source, g);
			update_dist(destination, g);
		}
	}
	fclose(f);

	/*print_node_array(g);
	print_edge_array(g);*/

	/*floyd_warshall(g);*/
	/*printf("%u.\n", g->dist[3][2]);
	printf("%u.\n", g->dist[2][1]);
	printf("%u.\n", g->dist[3][1]);
	printf("%u.\n", g->dist[4][1]);
	printf("%u.\n", g->dist[5][1]);
	printf("%u.\n", g->dist[5][0]);
	printf("%u.\n", g->dist[0][5]);
	printf("%u--.\n", g->dist[0][1]);
	printf("%u--.\n", g->dist[7][0]);
	printf("%u--.\n", g->dist[1][0]);
	printf("%u--.\n", g->dist[0][9]);*/

	/*unsigned int* a = g*et_out_neighbours(&g->node_array[0], g);
	free(a);
	a = get_out_neighbours(&g->node_array[1], g);
	free(a);
	a = get_in_neighbours(&g->node_array[0], g);
	free(a);
	unsigned int* b = get_in_neighbours(&g->node_array[1], g);
	free(b);*/

	/*update_dist(g->last_node->next, g);

	unsigned int a = get_index(g->last_node->next->next, g);
	printf("Index of %zu: %i.\n", g->last_node->next->next->index, a);*/

	/*print_nodes_v(g);
	print_edges_v(g);*/

	/*int* res;
	res = get_in_neighbours(g->last_node->next, g);
	printf("%zu.\n", g->last_node->next->index);
	printf("%i.\n", res[0]);
	printf("%i.\n", res[1]);
	printf("%i.\n", res[2]);
	printf("%i.\n", res[3]);
	printf("%i.\n", res[4]);
	printf("%i.\n", res[5]);
	printf("%i.\n", res[6]);

	printf("%i.\n", get_weight(g->last_node->next->next, g->last_node->next));
	*/

	/* Tests. */
	/*print_edge_array(g);
	print_node_array(g);

	printf("Manually adding.\n");
	char line[BUFSIZ] = "4 5 1 1231231231";
	buff = strtok(line, " ");
	add_edge(buff, g, add_source(buff, g), add_destination(buff, g));

	print_edge_array(g);
	print_node_array(g);
	printf("Removing the first node.\n");
	remove_node(g->last_node->next, g);

	printf("Removing the first edge.\n");
	remove_edge(g->last_edge->next, g);
	print_edge_array(g);
	print_node_array(g);

	printf("Manually adding.\n");
	char line2[BUFSIZ] = "4 5 2 1231231232";
	buff = strtok(line2, " ");
	add_edge(buff, g, add_source(buff, g), add_destination(buff, g));

	printf("Manually adding.\n");
	char line3[BUFSIZ] = "4 6 2 1231231240";
	buff = strtok(line3, " ");
	add_edge(buff, g, add_source(buff, g), add_destination(buff, g));
	print_nodes(g);
	print_edges(g);

	print_edge_array(g);
	print_node_array(g);

	printf("Manually adding.\n");
	char line4[BUFSIZ] = "6 6 2 1231231245";
	buff = strtok(line4, " ");
	add_edge(buff, g, add_source(buff, g), add_destination(buff, g));

	printf("Manually adding.\n");
	char line5[BUFSIZ] = "10 10 2 1231231249";
	buff = strtok(line5, " ");
	add_edge(buff, g, add_source(buff, g), add_destination(buff, g));

	printf("Manually adding.\n");
	char line6[BUFSIZ] = "10 9 2 1231231000";
	buff = strtok(line6, " ");
	add_edge(buff, g, add_source(buff, g), add_destination(buff, g));

	printf("Manually adding.\n");
	char line7[BUFSIZ] = "11 700 2 1231231001";
	buff = strtok(line7, " ");
	add_edge(buff, g, add_source(buff, g), add_destination(buff, g));

	printf("Manually adding.\n");
	char line8[BUFSIZ] = "11 9 2 1231231002";
	buff = strtok(line8, " ");
	add_edge(buff, g, add_source(buff, g), add_destination(buff, g));

	printf("Manually adding.\n");
	char line9[BUFSIZ] = "12 9 2 1231231003";
	buff = strtok(line9, " ");
	add_edge(buff, g, add_source(buff, g), add_destination(buff, g));

	printf("Manually adding.\n");
	char line10[BUFSIZ] = "13 9 2 1231231004";
	buff = strtok(line10, " ");
	add_edge(buff, g, add_source(buff, g), add_destination(buff, g));

	printf("Manually adding.\n");
	char line11[BUFSIZ] = "14 9 2 1231231005";
	buff = strtok(line11, " ");
	add_edge(buff, g, add_source(buff, g), add_destination(buff, g));

	printf("Manually adding.\n");
	char line12[BUFSIZ] = "15 9 2 1231231006";
	buff = strtok(line12, " ");
	add_edge(buff, g, add_source(buff, g), add_destination(buff, g));
	print_nodes(g);
	print_edges(g);

	print_edge_array(g);
	print_node_array(g);
	*/
	/* End of tests. */
	done_graph(g);

	return NO_ERROR;
}

