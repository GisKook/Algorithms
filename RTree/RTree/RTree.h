#ifndef RASTERISKTREE_H_H
#define RASTERISKTREE_H_H
/*
 * The R*-tree: An  Efficient  and Robust Access  Method 
 * Norbert  Beckmann,  Hans-Peter  begel 
 * Ralf Schneider,  Bernhard  Seeger 
 * Praktuche  Informatlk,  Umversltaet  Bremen,  D-2800  Bremen  33,  West  Germany 
 * 
 * Reference https://github.com/JasonBrownDeveloper/RTree.git 
 */

#include <stdint.h>
typedef int_fast32_t RTdimension;
typedef uint_fast8_t RTdimensionindex;
typedef uint_fast8_t RTchildindex;

#define RTREE_API 
#define RTn 2
#define RTPS 4096

struct RTNodeList {
   struct RTNodeList *Next;
   void *Tuple;
   RTdimension I[RTn*2];
};

bool RTNewTree(RTreePtr *T, struct RTNodeList *list);
bool RTSelectTuple(RTreePtr *T, RTdimension S[], struct RTNodeList **list, size_t *count);
bool RTSelectDimensions(RTreePtr *T, RTdimension I[]);
bool RTInsertTuple(RTreePtr *T, RTdimension I[], void *Tuple);
bool RTDeleteTuple(RTreePtr *T, RTdimension I[], void *Tuple);
bool RTUpdateTuple(RTreePtr *T, RTdimension I[], void *Tuple, void *New);
bool RTUpdateDimensions(RTreePtr *T, RTdimension I[], void *Tuple, RTdimension New[]);
bool RTFreeTree(RTreePtr *T);

#ifdef RTREE_DEBUG
bool RTTrace(struct RTNode *Start, size_t Level, size_t AbsChild, struct RTNode **Out);
bool RTDump(struct RTNode *Start, const char *filename);
#endif

#endif