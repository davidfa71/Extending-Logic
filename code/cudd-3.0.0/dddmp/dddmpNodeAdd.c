/**CFile**********************************************************************

  FileName     [dddmpNodeAdd.c]

  PackageName  [dddmp]

  Synopsis     [Functions to handle ADD node infos and numbering]

  Description  [Functions to handle ADD node infos and numbering.
    ]

  Author       [Gianpiero Cabodi and Stefano Quer]

  Copyright    [
    Copyright (c) 2004 by Politecnico di Torino.
    All Rights Reserved. This software is for educational purposes only.
    Permission is given to academic institutions to use, copy, and modify
    this software and its documentation provided that this introductory
    message is not removed, that this software and its documentation is
    used for the institutions' internal research and educational purposes,
    and that no monies are exchanged. No guarantee is expressed or implied
    by the distribution of this code.
    Send bug-reports and/or questions to:
    {gianpiero.cabodi,stefano.quer}@polito.it.
    ]

******************************************************************************/

#include "dddmpInt.h"

/*---------------------------------------------------------------------------*/
/* Stucture declarations                                                     */
/*---------------------------------------------------------------------------*/

/*
 *  StQ 01.08.2005
 *
 *  To deal with constants ... from cuddTable.c
 *  This is a hack for when CUDD_VALUE_TYPE is double
 *
 */

typedef union hack {
    CUDD_VALUE_TYPE value;
    unsigned int bits[2];
} hack;

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/

/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static int NumberNodeRecurAdd(DdNode *f, int id);
static void RemoveFromUniqueRecurAdd(DdManager *ddMgr, DdNode *f);
static void RestoreInUniqueRecurAdd(DdManager *ddMgr, DdNode *f);

/**AutomaticEnd***************************************************************/

/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis    [Removes nodes from unique table and number them]

  Description [Node numbering is required to convert pointers to integers.
    Since nodes are removed from unique table, no new nodes should 
    be generated before re-inserting nodes in the unique table
    (DddmpUnnumberAddNodes()).
    ]

  SideEffects [Nodes are temporarily removed from unique table]

  SeeAlso     [RemoveFromUniqueRecurAdd (), NumberNodeRecurAdd (), 
    DddmpUnnumberAddNodesAdd ()]

******************************************************************************/

int
DddmpNumberAddNodes (
  DdManager *ddMgr  /* IN: DD Manager */,
  DdNode **f        /* IN: array of BDDs */,
  int n             /* IN: number of BDD roots in the array of BDDs */
  )
{
  int id=0, i;

#if 0
  fprintf (stdout, "Before RemoveFromUniqueRecurBdd\n");
  My_PrintConstants (ddMgr);
#endif

  for (i=0; i<n; i++) {
    RemoveFromUniqueRecurAdd (ddMgr, f[i]);
  }

  for (i=0; i<n; i++) {
    id = NumberNodeRecurAdd (f[i], id);
  }

#if 0
  fprintf (stdout, "After RemoveFromUniqueRecurBdd\n");
  My_PrintConstants (ddMgr);
#endif

  return (id);
}


/**Function********************************************************************

  Synopsis     [Restores nodes in unique table, loosing numbering]

  Description  [Node indexes are no more needed. Nodes are re-linked in the
    unique table.
    ]

  SideEffects  [None]

  SeeAlso      [DddmpNumberDdNodeAdd ()]

******************************************************************************/

void
DddmpUnnumberAddNodes (
  DdManager *ddMgr  /* IN: DD Manager */,
  DdNode **f        /* IN: array of BDDs */,
  int n             /* IN: number of BDD roots in the array of BDDs */
  )
{
  int i;

#if 0
  fprintf ("Before RestoreInUniqueRecurBdd\n");
  My_PrintConstants (ddMgr);
#endif

  for (i=0; i<n; i++) {
    RestoreInUniqueRecurAdd (ddMgr, f[i]);
  }

#if 0
  fprintf ("After RestoreInUniqueRecurBdd\n");
  My_PrintConstants (ddMgr);
#endif

  return;
}

/**Function********************************************************************

  Synopsis     [Write index to node]

  Description  [The index of the node is written in the "next" field of
    a DdNode struct. LSB is not used (set to 0). It is used as 
    "visited" flag in DD traversals.
    ]

  SideEffects  [None]

  SeeAlso      [DddmpReadNodeIndexAdd (), DddmpSetVisitedAdd (),
    DddmpIsVisitedAdd ()]

******************************************************************************/

void 
DddmpWriteNodeIndexAdd (
  DdNode *f    /* IN: BDD node */,
  int id       /* IN: index to be written */
  )
{
  f->next = (struct DdNode *)((ptruint)((id)<<1));

  return;
}

/**Function********************************************************************

  Synopsis     [Reads the index of a node]

  Description  [Reads the index of a node. LSB is skipped (used as visited
    flag).
    ]

  SideEffects  [None]

  SeeAlso      [DddmpWriteNodeIndexAdd (), DddmpSetVisitedAdd  (),
    DddmpIsVisitedAdd ()]

******************************************************************************/

int
DddmpReadNodeIndexAdd (
  DdNode *f    /* IN: BDD node */
  )
{
  return ((int)(((ptruint)(f->next))>>1));
}

/**Function********************************************************************

  Synopsis     [Returns true if node is visited]

  Description  [Returns true if node is visited]

  SideEffects  [None]

  SeeAlso      [DddmpSetVisitedAdd (), DddmpClearVisitedAdd ()]

******************************************************************************/

int
DddmpIsVisitedAdd (
  DdNode *f    /* IN: BDD node to be tested */
  )
{
  f = Cudd_Regular (f);
  return ((int)((ptruint)(f->next)) & (01));
}

/**Function********************************************************************

  Synopsis     [Marks a node as visited]
 
  Description  [Marks a node as visited]

  SideEffects  [None]

  SeeAlso      [DddmpIsVisitedAdd (), DddmpClearVisitedAdd ()]

******************************************************************************/

void
DddmpSetVisitedAdd (
  DdNode *f   /* IN: BDD node to be marked (as visited) */
  )
{
  f = Cudd_Regular (f);

  f->next = (DdNode *)(ptruint)((int)((ptruint)(f->next))|01);

  return;
}

/**Function********************************************************************

  Synopsis     [Marks a node as not visited]

  Description  [Marks a node as not visited]

  SideEffects  [None]

  SeeAlso      [DddmpIsVisitedAdd (), DddmpSetVisitedAdd ()]

******************************************************************************/

void
DddmpClearVisitedAdd (
  DdNode *f    /* IN: BDD node to be marked (as not visited) */
  )
{
  f = Cudd_Regular (f);

  f->next = (DdNode *)(ptruint)((int)((ptruint)(f->next)) & (~01));

  return;
}

/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis     [Number nodes recursively in post-order]

  Description  [Number nodes recursively in post-order.
    The "visited" flag is used with inverse polarity, because all nodes
    were set "visited" when removing them from unique. 
    ]

  SideEffects  ["visited" flags are reset.]

  SeeAlso      []

******************************************************************************/

static int
NumberNodeRecurAdd (
  DdNode *f  /*  IN: root of the BDD to be numbered */,
  int id     /* IN/OUT: index to be assigned to the node */
  )
{
  f = Cudd_Regular (f);

  if (!DddmpIsVisitedAdd (f)) {
    return (id);
  }

  if (!cuddIsConstant (f)) {
    id = NumberNodeRecurAdd (cuddT (f), id);
    id = NumberNodeRecurAdd (cuddE (f), id);
  }

  DddmpWriteNodeIndexAdd (f, ++id);
  DddmpClearVisitedAdd (f);

  return (id);
}

/**Function********************************************************************

  Synopsis    [Removes a node from unique table]

  Description [Removes a node from the unique table by locating the proper
    subtable and unlinking the node from it. It recurs on the 
    children of the node. Constants remain untouched.
    ]

  SideEffects [Nodes are left with the "visited" flag true.]

  SeeAlso     [RestoreInUniqueRecurAdd ()]

******************************************************************************/

static void
RemoveFromUniqueRecurAdd (
  DdManager *ddMgr  /*  IN: DD Manager */,
  DdNode *f         /*  IN: root of the BDD to be extracted */
  )
{
  DdNode *node, *last, *next;
  DdNode *sentinel = &(ddMgr->sentinel);
  DdNodePtr *nodelist;
  DdSubtable *subtable;
  int pos, level;
  hack split;
  CUDD_VALUE_TYPE value;

  f = Cudd_Regular (f);

  if (DddmpIsVisitedAdd (f)) {
    return;
  }

  if (!cuddIsConstant (f)) {
    /* Non Constant Nodes */
    RemoveFromUniqueRecurAdd (ddMgr, cuddT (f));
    RemoveFromUniqueRecurAdd (ddMgr, cuddE (f));

    level = ddMgr->perm[f->index];
    subtable = &(ddMgr->subtables[level]);

    nodelist = subtable->nodelist;

    pos = ddHash (cuddT (f), cuddE (f), subtable->shift);
    node = nodelist[pos];
    last = NULL;
    while (node != sentinel) {
      next = node->next;
      if (node == f) {
        if (last != NULL)  
  	  last->next = next;
        else 
          nodelist[pos] = next;
        break;
      } else {
        last = node;
        node = next;
      }
    }

    f->next = NULL;
  } else {
    /* Constant Nodes */
    value = f->type.value;
    split.value = value;
    pos = ddHash (split.bits[0], split.bits[1], ddMgr->constants.shift);
    subtable = &(ddMgr->constants);
    nodelist = subtable->nodelist;
    node = nodelist[pos];
    last = NULL;
    while (node != NULL) {
      next = node->next;
      if (node == f) {
        if (last != NULL) {
          last->next = next;
        } else { 
          nodelist[pos] = next;
        }
        break;
      } else {
        last = node;
        node = next;
      }
    }

    f->next = NULL;
  }

  DddmpSetVisitedAdd (f);

  return;
}

/**Function********************************************************************

  Synopsis     [Restores a node in unique table]

  Description  [Restores a node in unique table (recursively)]

  SideEffects  [Nodes are not restored in the same order as before removal]

  SeeAlso      [RemoveFromUniqueRecurAdd ()]

******************************************************************************/

static void
RestoreInUniqueRecurAdd (
  DdManager *ddMgr /*  IN: DD Manager */,
  DdNode *f        /*  IN: root of the BDD to be restored */
  )
{
  DdNodePtr *nodelist;
  DdNode *T, *E, *looking;
  DdNodePtr *previousP;
  DdSubtable *subtable;
  int pos, level;
  hack split;
  CUDD_VALUE_TYPE value;
#ifdef DDDMP_DEBUG
  DdNode *node;
  DdNode *sentinel = &(ddMgr->sentinel);
#endif

  f = Cudd_Regular (f);

  if (!Cudd_IsComplement (f->next)) {
    return;
  }

  if (cuddIsConstant (f)) {
    /* StQ 11.02.2004 & 01.08.2005:
       Bug fixed --> restore NULL within the next field */
#if 0
    f->next = NULL;
#endif 
    value = f->type.value;
    split.value = value;
    pos = ddHash (split.bits[0], split.bits[1], ddMgr->constants.shift);
    subtable = &(ddMgr->constants);
    nodelist = subtable->nodelist;

    f->next = nodelist[pos];
    nodelist[pos] = f;

    return;
  }

  RestoreInUniqueRecurAdd (ddMgr, cuddT (f));
  RestoreInUniqueRecurAdd (ddMgr, cuddE (f));

  level = ddMgr->perm[f->index];
  subtable = &(ddMgr->subtables[level]);

  nodelist = subtable->nodelist;

  pos = ddHash (cuddT (f), cuddE (f), subtable->shift);

#ifdef DDDMP_DEBUG
  /* verify uniqueness to avoid duplicate nodes in unique table */
  for (node=nodelist[pos]; node != sentinel; node=node->next)
    assert (node!=f);
#endif

  T = cuddT (f);
  E = cuddE (f);
  previousP = &(nodelist[pos]);
  looking = *previousP;

  while (T < cuddT (looking)) {
    previousP = &(looking->next);
    looking = *previousP;
  }

  while (T == cuddT (looking) && E < cuddE (looking)) {
    previousP = &(looking->next);
    looking = *previousP;
  }

  f->next = *previousP;
  *previousP = f;

  return;
}



