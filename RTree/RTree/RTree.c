#include "RTree.h"

#define m 2                        /*minimum children; sizeof childindex*/
#define M (RTPS / sizeof(struct RTNode)) /*maximum children; sizeof childindex*/

#define LEVEL_TOP -1
#define LEVEL_LEAF 1
#define LEVEL_TUPLE 0

#define IS_BRANCH(N) ((N)->Child && (N)->Child[0].Child && (N)->Child[0].Tuple == NULL)
#define IS_LEAF(N)   ((N)->Child && (N)->Child[0].Tuple && (N)->Child[0].Child == NULL)
#define IS_TUPLE(N)  ((N)->Tuple && (N)->Child == NULL)
#define IS_EMPTY(N)  ((N).Parent == NULL) /* Will return true for root node */

static struct RTNode {
	struct RTNode *Parent;
	struct RTNode *Child;
	void *Tuple;
	RTdimension I[RTn*2]; /*{x1,y1,...,x2,y2...}*/
} EMPTY_NODE = {
	NULL,
	NULL,
	NULL,
	{0}
};


static void *mem_alloc(size_t _Size);

static bool Overlap(RTdimension *S1, RTdimension *S2);
static bool Within(RTdimension *S1, RTdimension *S2);
static long double safe_multiply(long double left, long double right);
static long double Volume(RTdimension *S);

static bool InitNodes(struct RTNode *ptr, RTchildindex size);
static bool FreeNodes(struct RTNode *T);

static bool Search(struct RTNode *T, RTdimension S[], struct RTNodeList **list, size_t *count);
static bool Insert(struct RTNode **N, size_t Level, RTdimension I[], void *Tuple, struct RTNode *Branch);
static bool ChooseLeaf(struct RTNode *N, size_t Start, size_t Stop, RTdimension *I, struct RTNode **leaf);
static bool AdjustTree(struct RTNode *N, struct RTNode *NN, struct RTNode **root, struct RTNode **split);
static bool Delete(struct RTNode **T, RTdimension I[], void *Tuple);
static bool FindLeaf(struct RTNode *T, RTdimension I[], void *Tuple, struct RTNode **L, RTchildindex *position);
static bool CondenseTree(struct RTNode *N, struct RTNode **root);
static bool LinearSplit(struct RTNode *L, RTdimension I[], void *Tuple, struct RTNode *Child, struct RTNode **split);
static bool LinearPickSeeds(struct RTNode NL[], long double *width, struct RTNode **hbest, struct RTNode **lbest);

/*Wrapper for malloc checks for out of memory*/
static void *mem_alloc(size_t size) {
   void *mem = malloc(size);
   if (!mem) {
      fputs("fatal: out of memory.\n", stderr);
      exit(EXIT_FAILURE);
   }
   return mem;
}

bool RTNewTree( RTreePtr *T, struct RTNodeList *list )
{
   size_t i, j, k;
   struct RTNodeList *nodelist;
   struct RTNode *node;

   struct RTNode *stack, *stack_next, *branch, *branch_next;

   stack = NULL;
   nodelist = list;
   while (nodelist != NULL) {
      node = (struct RTNode *)mem_alloc(sizeof(struct RTNode));
      InitNodes(node, 1);
      node->Child = (struct RTNode *)mem_alloc(M * sizeof(node->Child[0]));
      InitNodes(node->Child, M);

      memcpy(node->I, nodelist->I, sizeof(node->I));
      for (i = 0; nodelist != NULL && i < M; ++i) {
         memcpy(node->Child[i].I, nodelist->I, sizeof(node->Child[i].I));
         node->Child[i].Tuple = nodelist->Tuple;
         node->Child[i].Parent = node;

         for (j = 0, k = RTn; j < RTn; ++j, ++k) {
            node->I[j] = node->I[j] < nodelist->I[j] ? node->I[j] : nodelist->I[j];
            node->I[k] = node->I[k] > nodelist->I[k] ? node->I[k] : nodelist->I[k];
         }

         nodelist = nodelist->Next;
      }

      node->Parent = stack;
      stack = node;
   }

   while (stack && stack->Parent != NULL) {
      branch = stack;
      stack_next = NULL;
      while(branch != NULL) {
         node = (struct RTNode *)mem_alloc(sizeof(struct RTNode));
         InitNodes(node, 1);
         node->Child = (struct RTNode *)mem_alloc(M * sizeof((*T)->Child[0]));
         InitNodes(node->Child, M);

         memcpy(node->I, branch->I, sizeof(node->I));
         for (i = 0; branch != NULL && i < M; ++i) {
            branch_next = branch->Parent;

            memcpy(node->Child+i, branch, sizeof(node->Child[i]));
            node->Child[i].Parent = node;

            for (j = 0, k = RTn; j < RTn; ++j, ++k) {
               node->I[j] = node->I[j] < branch->I[j] ? node->I[j] : branch->I[j];
               node->I[k] = node->I[k] > branch->I[k] ? node->I[k] : branch->I[k];
            }

            free(branch);
            branch = branch_next;
         }
         node->Parent = stack_next;
         stack_next = node;
      }
      stack = stack_next;
   }

   (*T) = stack;

   if ((*T) == NULL) {
      (*T) = (struct RTNode *)mem_alloc(sizeof(struct RTNode));
      InitNodes(*T, 1);
      (*T)->Child = (struct RTNode *)mem_alloc(M * sizeof((*T)->Child[0]));
      InitNodes((*T)->Child, M);
   }

   return true;
}

/*Initializes an array of struct Nodes*/
static bool InitNodes(struct RTNode *ptr, RTchildindex size) {
	RTchildindex i;

	for (i = 0; i < size; ++i) {
		ptr[i] = EMPTY_NODE;
	}

	return true;
}