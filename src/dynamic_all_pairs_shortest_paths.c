#include "data_graph.h"
#include "dynamic_all_pairs_shortest_paths.h"


/* Floyd-Warshall's algorithm. */
/* Distance from node at array index i to node at array index j.
 * NOT from node i to node j.*/
void floyd_warshall(graph* g) {
	unsigned int i,j,k;

	edge* tmp = g->last_edge->next;
	do {
		g->dist[get_index(tmp->source,g)][get_index(tmp->destination,g)] =
						tmp->weight;
		/*printf("(%i,%i) = %i.\n", tmp->source->index, tmp->destination->index,
							g->dist[tmp->source->index][tmp->destination->index]);*/
		tmp = tmp->next;
	} while (tmp->prev != tmp);
	for (i = 0; i < g->nb_nodes; i++) {
		for (j = 0; j < g->nb_nodes; j++) {
			for (k = 0; k < g->nb_nodes; k++) {
				if (g->dist[i][j] > g->dist[i][k] + g->dist[k][j])
					g->dist[i][j] = g->dist[i][k] + g->dist[k][j];
				}
			/*printf("%i, %i = %i\n", i, j, g->dist[i][j]);*/
		}
	}
	printf("(%i,%i) = %i.\n", 6, 1, g->dist[6][1]);
}

unsigned int get_index(node* u, graph* g) {
	unsigned int i = 0;
	for (i = 0; i < g->nb_nodes; i++) {
		/*printf("Node array [%u] = %u, Node u: %u.\n", i, g->node_array[i].index,
		* u->index);*/
		if (g->node_array[i].index == u->index)
			return i;
	}
	printf("No such node.\n");
	return 0;
}

/* Returns an array of indices corresponding to the in-neighbours of node u in
 * g->node_array */
unsigned int* get_in_neighbours (node* u, graph* g){
	if (u->indeg != 0) {
		edge* tmp = u->firstin;
		unsigned int* res = calloc_wrapper(g->max_node_index,
																											sizeof(unsigned int));
		unsigned int i = 0;
		res[i++] = get_index(tmp->source, g);
		while (tmp != u->lastin) {
			tmp = tmp->targetnext;
			res[i++] = get_index(tmp->source, g);
		}
		return res;
	} else {
		return NULL;
	}
}

/* Returns an array of indices corresponding to the out-neighbours of node u in
 * g->node_array */
unsigned int* get_out_neighbours (node* u, graph* g){
	if (u->outdeg != 0) {
		edge* tmp = u->firstout;
		unsigned int* res = calloc_wrapper(g->max_node_index,
																											sizeof(unsigned int));
		unsigned int i = 0;
		res[i++] = get_index(tmp->destination, g);
		while (tmp != u->lastout) {
			tmp = tmp->sourcenext;
			res[i++] = get_index(tmp->destination, g);
		}
		return res;
	}
	return NULL;
}

int get_weight(node* u, node* v) {
	edge *tmp = u->firstout;
	/*printf("%zu lastout: %zu).\n", u->index, u->lastout->destination->index);*/
	do {
		/*printf("This is (%zu,%zu).\n", tmp->source->index,
																	tmp->destination->index);*/
		if (tmp->destination == v) {
			return tmp->weight;
		}
		tmp = tmp->sourcenext;
	} while (tmp->sourcenext != tmp);
	/*printf("This is (%zu,%zu).\n", tmp->source->index,
																	tmp->destination->index);*/
	if (tmp->destination == v) {
		return tmp->weight;
	}
	printf("Edge between %zu and %zu not found, weight is 100.\n", u->index,
									v->index);
	return 100;
}

void update_dist (node* u, graph* g) {
	unsigned int i, j;
	unsigned int*  in_neigh =  get_in_neighbours(u, g);
	unsigned int* out_neigh = get_out_neighbours(u, g);
	unsigned int u_ind = get_index(u, g);

	/*print_node_array(g);
	print_edge_array(g);
	printf("%zu has indegree %zu and outdegree %zu.\n", u->index, u->indeg,
									u->outdeg);
	printf("%zu.\n", g->node_array[u_ind].index);*/

	/*printf("Neigh[0] = %zu.\n", g->node_array[neigh[0]].index);
	printf("Updating outneighbour distance of %zu.\n", u->index);*/
	for (j = 0; j < u->outdeg; j++) {
		if (g->dist[u_ind][out_neigh[j]] >
															get_weight(u, &g->node_array[out_neigh[j]])) {
			/*printf("Outdistance from %zu to %tu set to %i as %i < %i.\n", u->index,
											g->node_array[out_neigh[j]].index,
											get_weight(u, &g->node_array[out_neigh[j]]),
											get_weight(u, &g->node_array[out_neigh[j]]),
											g->dist[u_ind][out_neigh[j]]);*/
			g->dist[u_ind][out_neigh[j]] = get_weight(u,
																								&g->node_array[out_neigh[j]]);
		/*(((X) < (Y)) ? (X) : (Y))*/
		}
	}
	/*printf("Updating inneighbour distance of %zu.\n", u->index);*/
	for (j = 0; j < u->indeg; j++) {
		if (g->dist[in_neigh[j]][u_ind] >
																get_weight(&g->node_array[in_neigh[j]], u)) {
			/*printf("Indistance from %zu to %tu set to %i as %i < %i.\n",
											g->node_array[in_neigh[j]].index, u->index,
											get_weight(&g->node_array[out_neigh[j]], u),
											get_weight(&g->node_array[out_neigh[j]], u),
											g->dist[in_neigh[j]][u_ind]);*/
			g->dist[in_neigh[j]][u_ind] = get_weight(&g->node_array[in_neigh[j]], u);
		}
	}

	/*printf("Updating distance from all vertices to %zu.\n", u->index);*/
	for (i = 0; i < g->max_node_index; i++) {
		for (j = 0; j < u->indeg; j++) {
			int a = g->dist[i][in_neigh[j]]
						+ get_weight(&g->node_array[in_neigh[j]],u);
			/* d(u,s) = min_k (d(u,k) + w(k,s)). */
			/*if (&g->node_array[out_neigh[j]] == u
					&& get_weight(&g->node_array[in_neigh[j]], u) < g->dist[i][u_ind]) {
				printf("Neighbour %zu is the current node %zu.\n",
																	g->node_array[in_neigh[j]].index, u->index);
				printf("Distance from %zu to %zu set to %i as %i < %i.\n",
												g->node_array[u_ind].index, g->node_array[i].index,
												get_weight(u, &g->node_array[in_neigh[j]]),
												get_weight(u, &g->node_array[in_neigh[j]]),
												g->dist[u_ind][i]);
				g->dist[i][u_ind] = get_weight(u, &g->node_array[in_neigh[j]]);
			}*/ if (a < g->dist[i][u_ind]) {
				/*printf("Distance from %zu to %zu set to %u as %i < %i.\n",
												g->node_array[i].index, g->node_array[u_ind].index,
												a, a, g->dist[i][u_ind]);*/
				g->dist[i][u_ind] = a;
			} else {
				/*printf("Not updating distance (%zu,%zu) as %i >= %i.\n",
												g->node_array[i].index, g->node_array[u_ind].index,
												a, g->dist[i][u_ind]);
				printf("i = %u, j = %u.\n", i, j);*/
			}
		}
	}
	/*printf("\n");
	printf("Updating distance from %zu to all vertices.\n", u->index);*/
	for (i = 0; i < g->max_node_index; i++) {
		for (j = 0; j < u->outdeg; j++) {
			int a = get_weight(u, &g->node_array[out_neigh[j]])
						+ g->dist[out_neigh[j]][i];
			/* d(s,u) = min_k (w(s,k) + d(k,u)). */
			/*if (&g->node_array[out_neigh[j]] == u
					&& get_weight(u,&g->node_array[out_neigh[j]]) < g->dist[u_ind][i]) {
				g->dist[i][u_ind] = get_weight(u, &g->node_array[out_neigh[j]]);
				printf("Neighbour %zu is the current node %zu.\n",
																g->node_array[out_neigh[j]].index, u->index);
				printf("Distance from %zu to %zu set to %i as %i < %i.\n",
												g->node_array[u_ind].index, g->node_array[i].index,
												get_weight(u, &g->node_array[out_neigh[j]]),
												get_weight(u, &g->node_array[out_neigh[j]]),
												g->dist[u_ind][i]);
			}*/if (a < g->dist[u_ind][i]) {
				/*printf("Distance from %zu to %zu set to %u as %i < %i.\n",
												g->node_array[u_ind].index, g->node_array[i].index,
												a, a, g->dist[u_ind][i]);*/
				g->dist[u_ind][i] = a;
			} else {
				/*printf("Not updating distance (%zu,%zu) as %i+%i >= %i.\n",
												g->node_array[u_ind].index, g->node_array[i].index,
												get_weight(u, &g->node_array[out_neigh[j]]),
												g->dist[out_neigh[j]][i], g->dist[u_ind][i]);
				printf("i = %u, j = %u, out_neigh[j] = %i.\n", i, j, out_neigh[j]);
				printf("out_neigh[j] = %zu.\n", g->node_array[out_neigh[j]].index);*/
			}/*g->dist[i][u_ind] = g->dist[i][neigh[j]] +
			* get_weight(&g->node_array[neigh[j]],u);*/
		}
	}
	free(in_neigh);
	free(out_neigh);

	/*printf("\n");*/
	for (i = 0; i < g->max_node_index; i++) {
		for (j = 0; j < g->max_node_index; j++) {
			int a = g->dist[i][u_ind] + g->dist[u_ind][j];
			if (a < g->dist[i][j]) {
				/*printf("Distance from %zu to %zu set to %i as %i+%i < %i.\n",
												g->node_array[i].index, g->node_array[j].index,
												a, g->dist[i][u_ind], g->dist[u_ind][j],
												g->dist[i][j]);*/
				g->dist[i][j] = a;
			} else {
				/*printf("Not updating distance (%zu,%zu) as %i+%i >= %i.\n",
												g->node_array[i].index, g->node_array[j].index,
												g->dist[i][u_ind], g->dist[u_ind][j], g->dist[i][j]);*/
			}
		}
	}
	/*printf("\n");*/
}

