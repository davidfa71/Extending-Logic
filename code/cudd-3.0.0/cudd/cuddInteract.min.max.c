/**
  @file

  @ingroup cudd

  @brief Functions to manipulate the variable interaction matrix.

  @details The interaction matrix tells whether two variables are both
  in the support of some function of the %DD. The main use of the
  interaction matrix is in the in-place swapping. Indeed, if two
  variables do not interact, there is no arc connecting the two
  layers; therefore, the swap can be performed in constant time,
  without scanning the subtables. Another use of the interaction
  matrix is in the computation of the lower bounds for
  sifting. Finally, the interaction matrix can be used to speed up
  aggregation checks in symmetric and group sifting.<p>
  The computation of the interaction matrix is done with a series of
  depth-first searches. The searches start from those nodes that have
  only external references. The matrix is stored as a packed array of
  bits; since it is symmetric, only the upper triangle is kept in
  memory.  As a final remark, we note that there may be variables that
  do interact, but that for a given variable order have no arc
  connecting their layers when they are adjacent.  For instance, in
  ite(a,b,c) with the order a<b<c, b and c interact, but are not
  connected.

  @author Fabio Somenzi

  @copyright@parblock
  Copyright (c) 1995-2015, Regents of the University of Colorado

  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

  Neither the name of the University of Colorado nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
  @endparblock

*/

#include "util.h"
#include "cuddInt.h"

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/

#if SIZEOF_VOID_P == 8
#define BPL 64
#define LOGBPL 6
#else
#define BPL 32
#define LOGBPL 5
#endif

/*---------------------------------------------------------------------------*/
/* Stucture declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/

/** \cond */

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static void ddSuppInteract (DdManager * table, DdNode *f, char *support);
static void ddClearLocal (DdManager * table, DdNode *f);
static void ddUpdateInteract (DdManager *table, char *support);
static void ddClearGlobal (DdManager *table);

/** \endcond */


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


/**
  @brief Set interaction matrix entries.

  @details Given a pair of variables 0 <= x < y < table->size,
  sets the corresponding bit of the interaction matrix to 1.

  @sideeffect None

*/
void
cuddSetInteract(
  DdManager * table,
  int  x,
  int  y)
{
    ptruint posn, word, bit;

#ifdef DD_DEBUG
    assert(x < y);
    assert(y < table->size);
    assert(x >= 0);
#endif
    int min  = table->interact[0];
    int max  = table->interact[1];
    int posx = table->perm[x];
    int posy = table->perm[y];
    //fprintf(stderr, "P0 setInteract x=%d, y=%d, posx=%d, posy=%d\n", x, y, posx, posy);
    if (posx > posy) {
	int tempos = posx;
	posx = posy;
	posy = tempos;
    }
    //fprintf(stderr, "P1 setInteract x=%d, y=%d, posx=%d, posy=%d\n", x, y, posx, posy);
    // posn = (((((ptruint)table->size << 1) - x - 3) * x) >> 1) + y - 1;
    posn = (((((ptruint)(max-min+1) << 1) - (posx-min) - 3) * (posx-min)) >> 1) + (posy-min) - 1;
    word = posn >> LOGBPL;
    bit = posn & (BPL-1);
    //fprintf(stderr,"word=%d\n", (int) word);
    table->interact[word+2] |= ((ptruint) 1) << bit;

} /* end of cuddSetInteract */


/**
  @brief Test interaction matrix entries.

  @details Given a pair of variables 0 <= x < y < table->size,
  tests whether the corresponding bit of the interaction matrix is 1.
  Returns the value of the bit.

  @sideeffect None

*/
int
cuddTestInteract(
  DdManager * table,
  int  x,
  int  y)
{
  int min = table->interact[0];
  int max = table->interact[1];
  int posx = table->perm[x];
  int posy = table->perm[y];
     //fprintf(stderr, "P0 testInteract x=%d, y=%d, posx=%d, posy=%d\n", x, y, posx, posy);
  ptruint posn, word, bit;
  int result;

    if (posx > posy) {
        int tmpos = posx;
	posx = posy;
	posy = tmpos;
    }
//#ifdef DD_DEBUG
//    assert(x < y);
//    assert(y < table->size);
//    assert(x >= 0);
//#endif
    // posn = (((((ptruint)table->size << 1) - x - 3) * x) >> 1) + y - 1;
    posn = (((((ptruint)(max-min+1) << 1) - (posx-min) - 3) * (posx-min)) >> 1) + (posy-min) - 1;
    word = posn >> LOGBPL;
    bit = posn & (BPL-1);
    // DFA We add 2 because positions 0 and 1 now store min and max variables
    result = (table->interact[word+2] >> bit) & (ptruint) 1;
    return(result);

} /* end of cuddTestInteract */


/**
  @brief Initializes the interaction matrix.

  @details The interaction matrix is implemented as a bit vector
  storing the upper triangle of the symmetric interaction matrix. The
  bit vector is kept in an array of ptruints. The computation is based
  on a series of depth-first searches, one for each root of the
  DAG. Two flags are needed: The local visited flag uses the LSB of
  the then pointer. The global visited flag uses the LSB of the next
  pointer.

  @return 1 if successful; 0 otherwise.

  @sideeffect None

*/

// min and max are levels (positions)
int
cuddInitInteractMinMax(
                 DdManager * table, int min, int max)
{
    unsigned int i;
    int j;
    ptruint words;
    ptruint *interact;
    char *support;
    DdNode *f;
    DdNode *sentinel = &(table->sentinel);
    DdNodePtr *nodelist;
    int slots;
    min = 0;
    max = table->size -1;
    ptruint n = (ptruint) max - min + 1;
    
    words = ((n * (n-1)) >> (1 + LOGBPL)) + 1 + 2;
    table->interact = interact = ALLOC(ptruint,words);
    if (interact == NULL) {
        table->errorCode = CUDD_MEMORY_OUT;
        return(0);
    }
    interact[0] = min;
    interact[1] = max;
    // fprintf(stderr, "P0 initInteractMinMax min=%d, max=%d words=%d\n", min, max, (int)words);
    for (i = 2; i < words; i++) {
        interact[i] = 0;
    }
    
    support = ALLOC(char,max - min +1);
    if (support == NULL) {
        table->errorCode = CUDD_MEMORY_OUT;
        FREE(interact);
        return(0);
    }
    for (i = min; i < max+1; i++) {
        support[i-min] = 0;
    }
    
   for (i = 0; i < table->size; i++) {
        int vari = table->invperm[i];
        nodelist = table->subtables[vari].nodelist;
        slots = table->subtables[vari].slots;
	//fprintf(stderr, "P0 Variable %d, position %d, min %d, max %d slots=%d\n", vari, i , min, max, slots);	
        for (j = 0; j < slots; j++) {
            f = nodelist[j];
            while (f != sentinel) {
                /* A node is a root of the DAG if it cannot be
                 ** reached by nodes above it. If a node was never
                 ** reached during the previous depth-first searches,
                 ** then it is a root, and we start a new depth-first
                 ** search from it.
                 */
                //if (table->perm[f->index] < i)
		//   fprintf(stderr, "f=%p level=%d less than %d\n",f, table->perm[f->index], i );
		//fprintf(stderr, "P1 f=%p, f->index=%d\n", f, f->index);
                if (!Cudd_IsComplement(f->next)) {
                    ddSuppInteract(table, f,support);
                    ddClearLocal(table, f);
                    ddUpdateInteract(table,support);
                }
                f = Cudd_Regular(f->next);
            }
        }
    }
    ddClearGlobal(table);
    
    FREE(support);
    return(1);
    
}
            
int cuddInitInteract(DdManager * table)
    {
        return cuddInitInteractMinMax(table, 0 , table->size - 1);
        
    } /* end of cuddInitInteract */
/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/


/**
  @brief Find the support of f.

  @details Performs a DFS from f. Uses the LSB of the then pointer
  as visited flag.

  @sideeffect Accumulates in support the variables on which f depends.

*/
static void
ddSuppInteract(
  DdManager * table,
  DdNode * f,
  char * support)
{
    int min = table->interact[0];
    int max = table->interact[1];
    if (cuddIsConstant(f) || Cudd_IsComplement(cuddT(f))) {
	return;
    }
    if ( table->perm[f->index] >= min &&  table->perm[f->index] <= max)
    	support[table->perm[f->index] -min] = 1;

    ddSuppInteract(table, cuddT(f),support);
    ddSuppInteract(table, Cudd_Regular(cuddE(f)),support);
    /* mark as visited */
    cuddT(f) = Cudd_Complement(cuddT(f));
    f->next = Cudd_Complement(f->next);
    return;

} /* end of ddSuppInteract */


/**
  @brief Performs a DFS from f, clearing the LSB of the then pointers.

  @sideeffect None

*/
static void
ddClearLocal(
  DdManager * table,
  DdNode * f)
{
    int max = table->interact[1];
    if (cuddIsConstant(f) || !Cudd_IsComplement(cuddT(f)) || table->perm[f->index] > max) {
	return;
    }
    /* clear visited flag */
    cuddT(f) = Cudd_Regular(cuddT(f));
    ddClearLocal(table, cuddT(f));
    ddClearLocal(table, Cudd_Regular(cuddE(f)));
    return;

} /* end of ddClearLocal */


/**
  @brief Marks as interacting all pairs of variables that appear in
  support.

  @details If support[i == support[j] == 1, sets the (i,j) entry
  of the interaction matrix to 1.]

  @sideeffect Clears support.

*/
static void
ddUpdateInteract(
  DdManager * table,
  char * support)
{
    int min = table->interact[0];
    int max = table->interact[1];
    int i,j;
    int n = max - min + 1;

    for (i = min; i < max; i++) {
	if (support[i - min ] == 1) {
            support[i - min ] = 0;
	    for (j = i+1; j < max+1; j++) {
		if (support[j-min] == 1) {
            int vari = table->invperm[i];
            int varj = table->invperm[j];
		    cuddSetInteract(table, vari, varj);
		}
	    }
	}
    }
    support[max - min] = 0;

} /* end of ddUpdateInteract */


/**
  @brief Scans the %DD and clears the LSB of the next pointers.

  @details The LSB of the next pointers are used as markers to tell
  whether a node was reached by at least one DFS. Once the interaction
  matrix is built, these flags are reset.

  @sideeffect None

*/
static void
ddClearGlobal(
  DdManager * table)
{
    int min = table->interact[0];
    int max = table->interact[1];
    int i,j;
    DdNode *f;
    DdNode *sentinel = &(table->sentinel);
    DdNodePtr *nodelist;
    int slots;

    for (i = min; i < max+1; i++) {
        int vari = table->invperm[i];
        nodelist = table->subtables[vari].nodelist;
        slots = table->subtables[vari].slots;
        for (j = 0; j < slots; j++) {
            f = nodelist[j];
            while (f != sentinel) {
                f->next = Cudd_Regular(f->next);
                f = f->next;
            }
        }
    }

} /* end of ddClearGlobal */

