struct rxtconfig{ 
	unsigned int M;
};

struct rxtroot{ 
	int index[4];
	struct rxtnode * child;
};

struct rxtree{
	struct rxtconfig * config;
};

static struct rxtnode {
	struct rxtnode *parent;
	struct rxtnode *child;
	struct rxtnode *next;
	void *tuple;
	int index[4]; /*{x1,y1,...,x2,y2...}*/
} EMPTY_NODE = {
	NULL,
	NULL,
	NULL,
	NULL,
	{0}
};

static void *mem_alloc(size_t);

struct rxtnode * create(int index[4],void * tuple){ 
	struct rxtnode *root = mem_alloc(sizeof(struct rxtnode));
	*root = EMPTY_NODE;
	root->tuple = tuple;
	memcpy(root->index, index, sizeof(root->index));

	return root;
}

struct rxtnode * insert(struct rxtnode * root, struct rxtnode * element){

}

static void *mem_alloc(size_t size) {
	void *mem = malloc(size);
	if (!mem) {
		fprintf(stdout,"fatal: out of memory.\n");
		exit(EXIT_FAILURE);
	}
	return mem;
}