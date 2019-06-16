#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX_SIZE 100

// stack	{{{
typedef struct Stack{
	int* st;
	int top;
} Stack;

void stack_init(Stack* S){S->top = 0; S->st = (int*)malloc(MAX_SIZE * sizeof(int));}
int isEmpty(Stack* S){return S->top == 0;}
int isFull(Stack* S){return S->top == MAX_SIZE;}
void push(Stack* S, int val){assert(!isFull(S)); S->st[S->top++] = val;}
int pop(Stack* S){assert(!isEmpty(S)); return S->st[--S->top];}
//	}}}

// adjacency list {{{
typedef struct adjlistnode{
	int index;
	struct adjlistnode* next;
} lstnode;

void adjlst_init(lstnode** adjlst){
	*adjlst = (lstnode*)malloc(MAX_SIZE * sizeof(lstnode));
	for(int i = 0; i < MAX_SIZE; ++i){
		(*adjlst)[i].next = NULL;
	}
}
void build_edge_sub(lstnode* adjlst, int u, int v){
	lstnode* newnode = (lstnode*)malloc(sizeof(lstnode));
	assert(newnode != NULL);
	newnode->index = v;
	newnode->next = adjlst[u].next;
	adjlst[u].next = newnode;
}
//}}}

// graph	{{{
typedef struct Graph{
	int size;
	lstnode* adj_lst; 
} Graph;
void graph_init(Graph* G, int V){
	G->size = V;
	adjlst_init(&(G->adj_lst));
}
void build_edge(Graph* G, int u, int v){
	build_edge_sub(G->adj_lst, u, v);
}
Graph* reverse_graph(Graph* G){
	Graph* R = (Graph*)malloc(sizeof(Graph));
	int n = G->size;
	graph_init(R, n);
	
	for(int i = 0; i < n; ++i){
		lstnode* ptr = G->adj_lst[i].next;
		while(ptr != NULL){
			build_edge(R, ptr->index, i);
			ptr = ptr->next;
		}
	}
	return R;
}
//	}}}

// DFS	{{{
void DFS(Graph* G, int start, int* visited, Stack* S){
	visited[start] = 1;
	lstnode* ptr = G->adj_lst[start].next;
	while(ptr != NULL){
		if(!visited[ptr->index]) DFS(G, ptr->index, visited, S);
		ptr = ptr->next;
	}
	push(S, start);
}

void DFS_reverse(Graph* G, int start, int* visited, int* SCC, int SCC_index){
	visited[start] = 1;
	SCC[start] = SCC_index;
	lstnode* ptr = G->adj_lst[start].next;
	while(ptr != NULL){
		if(!visited[ptr->index]) DFS_reverse(G, ptr->index, visited, SCC, SCC_index);
		ptr = ptr->next;
	}
}
//}}}

/*	Kosaraju's algorithm 
 *		input:	a Graph G
 *		output: an array SCC indicating the mapping of original vertex in G 
 *				to its corresponding Strongly Connected Component.
 */
int Kosaraju(int* SCC, Graph* G){{{
	int n = G->size;	// # of vertices in G 

	int* visited = (int*)calloc(n, sizeof(int));
	Stack* S = (Stack*)malloc(sizeof(Stack));
	stack_init(S);
	
	for(int i = 0; i < n; ++i){
		if(!visited[i]){
			DFS(G, i, visited, S);
		}
	}
	
	memset(visited, 0, n*sizeof(int));
	Graph* G_R = reverse_graph(G);

	int x, SCC_index = 0;
	while(!isEmpty(S)){
		x = pop(S);
		if(!visited[x]){
			DFS_reverse(G_R, x, visited, SCC, SCC_index);
			++SCC_index;
		}
	}
	
	return SCC_index;
}}}

int main(){
	/*	V: # of vertices
	 *	E: # of edges
	 */
	int V, E;
	scanf("%d %d", &V, &E);


	Graph* G = (Graph*)malloc(sizeof(Graph));
	graph_init(G, V);

	
	int u, v;
	for(int i = 0; i < E; ++i){
		/*	each pair of <u, v> means
		 *	there is a directed edge
		 *	from u to v
		 */
		scanf("%d %d", &u, &v);
		build_edge(G, u, v);
	}

	//	mapping from original index to the index in reduced graph
	int* SCC = (int*)malloc(V * sizeof(int));
	int SCC_count = 0;

	SCC_count = Kosaraju(SCC, G);

	//	print the result to stdout
	printf("# of SCCs: %d\n", SCC_count);
	for(int i = 0; i < V; ++i){
		printf("SCC[%5d]:\t%d\n", i, SCC[i]);
	}

	return 0;
}
