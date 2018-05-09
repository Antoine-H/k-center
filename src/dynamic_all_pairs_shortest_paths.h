#ifndef __APSP__
#define __APSP__

void floyd_warshall(graph* g);

unsigned int get_index(node* u, graph* g);
unsigned int* get_in_neighbours (node* u, graph* g);
unsigned int* get_out_neighbours (node* u, graph* g);
int get_weight(node* u, node* v);

void update_dist (node* u, graph* g);

#endif

