/* Copyright (c) 2002-2003 by John M. Boyer, All Rights Reserved.
        This code may not be reproduced or disseminated in whole or in part
        without the written permission of the author. */

#include "graphK33Search.h"
#include "graphK33Search.private.h"

#include "graph.h"

/* Imported functions */

extern void _FillVisitedFlags(graphP, int);
extern void _FillVisitedFlagsInBicomp(graphP theGraph, int BicompRoot, int FillValue);
extern void _FillVisitedFlagsInOtherBicomps(graphP theGraph, int BicompRoot, int FillValue);
extern void _FillVisitedFlagsInUnembeddedEdges(graphP theGraph, int FillValue);
extern int  _GetBicompSize(graphP theGraph, int BicompRoot);
extern void _HideInternalEdges(graphP theGraph, int vertex);
extern void _RestoreInternalEdges(graphP theGraph);
extern void _DeleteUnmarkedEdgesInBicomp(graphP theGraph, int BicompRoot);
extern void _ClearInvertedFlagsInBicomp(graphP theGraph, int BicompRoot);

extern int  _GetNextVertexOnExternalFace(graphP theGraph, int curVertex, int *pPrevLink);
extern int  _WalkUp(graphP theGraph, int I, int W);
extern int  _JoinBicomps(graphP theGraph);
extern void _OrientVerticesInBicomp(graphP theGraph, int BicompRoot, int PreserveSigns);
extern void _OrientVerticesInEmbedding(graphP theGraph);
extern void _InvertVertex(graphP theGraph, int V);

extern int  _ChooseTypeOfNonplanarityMinor(graphP theGraph, int I, int R);
extern int  _MarkHighestXYPath(graphP theGraph);

extern int  _IsolateKuratowskiSubgraph(graphP theGraph, int I);

extern int  _FindUnembeddedEdgeToCurVertex(graphP theGraph, int cutVertex, int *pDescendant);
extern int  _FindUnembeddedEdgeToSubtree(graphP theGraph, int ancestor, int SubtreeRoot, int *pDescendant);

extern int  _MarkPathAlongBicompExtFace(graphP theGraph, int startVert, int endVert);

extern int  _AddAndMarkEdge(graphP theGraph, int ancestor, int descendant);

extern int  _DeleteUnmarkedVerticesAndEdges(graphP theGraph);

extern int  _IsolateMinorE1(graphP theGraph);
extern int  _IsolateMinorE2(graphP theGraph);
extern int  _IsolateMinorE3(graphP theGraph);
extern int  _IsolateMinorE4(graphP theGraph);

extern int  _GetLeastAncestorConnection(graphP theGraph, int cutVertex);
extern int  _MarkDFSPathsToDescendants(graphP theGraph);
extern int  _AddAndMarkUnembeddedEdges(graphP theGraph);

/* Private functions for K3,3 searching. */

int  _SearchForK33(graphP theGraph, int I);

int  _SearchForK33InBicomp(graphP theGraph, K33SearchContext *context, int I, int R);

int  _RunExtraK33Tests(graphP theGraph, K33SearchContext *context);
int  _SearchForMinorE1(graphP theGraph);
int  _FinishIsolatorContextInitialization(graphP theGraph, K33SearchContext *context);
int  _SearchForDescendantExternalConnection(graphP theGraph, K33SearchContext *context, int cutVertex, int u_max);
int  _GetAdjacentAncestorInRange(graphP theGraph, K33SearchContext *context, int vertex,
                                int closerAncestor, int fartherAncestor);
int  _FindExternalConnectionDescendantEndpoint(graphP theGraph, int ancestor,
                                               int cutVertex, int *pDescendant);
int  _SearchForMergeBlocker(graphP theGraph, K33SearchContext *context, int I, int *pMergeBlocker);
int  _FindK33WithMergeBlocker(graphP theGraph, K33SearchContext *context, int I, int mergeBlocker);

int  _TestForLowXYPath(graphP theGraph);
int  _TestForZtoWPath(graphP theGraph);
int  _TestForStraddlingBridge(graphP theGraph, K33SearchContext *context, int u_max);
int  _ReduceBicomp(graphP theGraph, K33SearchContext *context, int R);
int  _ReduceExternalFacePathToEdge(graphP theGraph, K33SearchContext *context, int u, int x, int edgeType);
int  _ReduceXYPathToEdge(graphP theGraph, K33SearchContext *context, int u, int x, int edgeType);
int  _RestoreReducedPath(graphP theGraph, K33SearchContext *context, int J);
int  _RestoreAndOrientReducedPaths(graphP theGraph, K33SearchContext *context);
int  _SetEdgeType(graphP theGraph, int u, int v);
int  _OrientPath(graphP theGraph, int u, int v, int w, int x);
void _SetVisitedOnPath(graphP theGraph, int u, int v, int w, int x, int visited);

int  _IsolateMinorE5(graphP theGraph);
int  _IsolateMinorE6(graphP theGraph, K33SearchContext *context);
int  _IsolateMinorE7(graphP theGraph, K33SearchContext *context);

/****************************************************************************
 _SearchForK33()

  The strategy involves one special case in which, to achieve a linear time
  bound, we must delay the discovery of a K_{3,3} that caused a Walkdown
  failure prior to step I.  In such cases, vertex I was an ancestor with
  a connection to the bicomp on which the Walkdown failed, but it would
  have been too costly to find I at the time.  So, the bicomp was marked
  as non-mergeable prior to some ancestor of I.  If this function is
  invoked for step I, then we have found the connection from that bicomp
  prior to reaching the limiting ancestor of I. The bicomp and I are
  therefore part of a K_{3,3} that can be isolated.

  Otherwise, a Walkdown failure in step I with a non-empty merge stack
  would have already resulted in an identified K_{3,3} by minor A, so
  we must have an empty merge stack now.

  We must first find all bicomp roots on which the Walkdown has failed
  in step I.  The fwdArcList of I contains the forward arcs of the
  back edges for I that we failed to embed.  Each forward arc leads to
  a descendant of I that is in a DFS subtree rooted by a child of I,
  where the child of I has the greatest DFI that is less than the DFI
  of the descendant indicated by the forward arc.  Each bicomp root
  that represents a vertex is uniquely associated with a DFS child
  of the vertex, so once we know the child of I whose subtree contains
  a descendant of I that the Walkdown couldn't reach, we can immediately
  deduce the root copy of I on which the Walkdown failed.

  For each such root copy of I, we test whether a K_{3,3} homemorph
  can be isolated based on that bicomp.  If so, then we return it.
  Otherwise, each bicomp can be reduced to a 4-cycle and the edges
  that the Walkdown failed to embed can be discarded.
 ****************************************************************************/

int  _SearchForK33(graphP theGraph, int I)
{
int  C1, C2, D, e, RetVal=NOTOK;
K33SearchContext *context = NULL;

    gp_FindExtension(theGraph, "K33Search", (void *)&context);
    if (context == NULL)
        return NOTOK;

/* Before we begin with the standard array of K3,3 tests, we handle
    one optimization case that may be left over from a prior step
    of the embedding algorithm.  If the embedding stack is non-empty,
    then the Walkdown either halted due to non-planarity minor A or
    because of the merge blocking optimization (see CASE 3 in the
    function RunExtraK33Tests()).  We test for the latter condition,
    and if it is found, then we isolate a K3,3 and return. */

     if (!sp_IsEmpty(theGraph->theStack))
     {
     int mergeBlocker;

         if (_SearchForMergeBlocker(theGraph, context, I, &mergeBlocker) != OK)
             return NOTOK;

         if (mergeBlocker != NIL)
         {
             if (_FindK33WithMergeBlocker(theGraph, context, I, mergeBlocker) != OK)
                 return NOTOK;

             return NONEMBEDDABLE;
         }
     }

     /* Each DFS child is listed in DFI order in V[I].sortedDFSChildList.
        In V[I].fwdArcList, the forward arcs of all unembedded back edges are
        in order by DFI of the descendant endpoint of the edge.

        DFS descendants have a higher DFI than ancestors, so given two
        successive children C1 and C2, if any forward arcs lead to a
        vertex D such that DFI(C1) < DFI(D) < DFI(C2), then the Walkdown
        failed to embed a back edge from I to a descendant D of C1. */

     e = theGraph->V[I].fwdArcList;
     D = theGraph->G[e].v;

     C1 = context->V[I].sortedDFSChildList;

     while (C1 != NIL && e != NIL)
     {
        C2 = LCGetNext(context->sortedDFSChildLists,
                       context->V[I].sortedDFSChildList, C1);

        // If the edge e leads from I to a descendant D of C1,
        // then D will be less than C2 (as explained above),
        // so we search for a K_{3,3} in the bicomp rooted
        // by the root copy of I associated with C1.
        // (If C2 is NIL, then C1 is the last child)

        if (D < C2 || C2 == NIL)
        {
            RetVal = _SearchForK33InBicomp(theGraph, context, I, C1+theGraph->N);

            // If something went wrong, NOTOK was returned;
            // If a K_{3,3} was found, NONEMBEDDABLE was returned;
            // If OK was returned, then only a K5 was found, so
            // we continue searching any other bicomps on which
            // the Walkdown failed.

            if (RetVal != OK)
             break;
        }

        // Skip the edges that lead to descendants of C1 to get to those
        // edges that lead to descendants of C2.

        if (C2 != NIL)
        {
            while (D < C2 && gp_IsEdge(theGraph, e))
            {
                e = gp_GetNextEdge(theGraph, e);
                if (e == theGraph->V[I].fwdArcList)
                     e = NIL;
                else D = theGraph->G[e].v;
            }
        }

        // Move the DFS child context to C2
        C1 = C2;
     }

/* If we got through the loop with an OK value for each bicomp on
     which the Walkdown failed, then we return OK to indicate that only
     K5's were found (or there is a special case K3,3 that may be discovered
     later based on a setting we made in the data structure).
     The OK return allows the embedder to continue.

     If a K3,3 is ever found (or if an error occured), then RetVal
     will not be OK, and the loop terminates immediately so we can
     return the appropriate value.  If a K3,3 is found, then we must
     also handle the fact that some paths of the input graph may have
     been reduced to single edges by prior _ReduceBicomp() calls.

     NOTE: RetVal starts out NOTOK to ensure we detect at least one
        bicomp on which the Walkdown failed (this should always be
        the case in an error-free implementation like this one!). */

     return RetVal;
}

/****************************************************************************
 _SearchForK33InBicomp()
 ****************************************************************************/

int  _SearchForK33InBicomp(graphP theGraph, K33SearchContext *context, int I, int R)
{
isolatorContextP IC = &theGraph->IC;
int tempResult;

/* Begin by determining which non-planarity minor is detected */

     if (_ChooseTypeOfNonplanarityMinor(theGraph, I, R) != OK)
         return NOTOK;

/* If minor A is selected, then the root of the oriented bicomp has been changed */

     else R = IC->r;

/* Minors A to D result in the desired K3,3 homeomorph,
    so we isolate it and return NONEMBEDDABLE. */

     if (theGraph->IC.minorType & (MINORTYPE_A|MINORTYPE_B|MINORTYPE_C|MINORTYPE_D))
     {
        /* First we restore the orientations of the vertices in the
            one bicomp we have messed with so that there is no confusion. */

        _OrientVerticesInBicomp(theGraph, R, 1);

        /* Next we restore the orientation of the embedding so we
           can restore the reduced paths (because we avoid modifying
           the Kuratowski subgraph isolator to restore reduced paths,
           which are a construct of the K3,3 search). */

        _OrientVerticesInEmbedding(theGraph);
        if (_RestoreAndOrientReducedPaths(theGraph, context) != OK)
            return NOTOK;

        /* Next we simply call the Kuratowski subgraph isolation since
            we know now that it will isolate a K3,3.
            For minor A, we need to set up the stack that would be
            available immediately after a Walkdown failure. */

        if (theGraph->IC.minorType & MINORTYPE_A)
        {
            sp_ClearStack(theGraph->theStack);
            sp_Push2(theGraph->theStack, R, NIL);
        }

        if (_IsolateKuratowskiSubgraph(theGraph, I) != OK)
            return NOTOK;

        return NONEMBEDDABLE;
     }

/* For minor E (a K5 minor), we run the additional tests to see if
    minors E1 to E4 apply since these minors isolate a K3,3 entangled
    with the K5. */

     IC->ux = _GetLeastAncestorConnection(theGraph, IC->x);
     IC->uy = _GetLeastAncestorConnection(theGraph, IC->y);
     IC->uz = _GetLeastAncestorConnection(theGraph, IC->z);

     if (IC->z != IC->w ||
         IC->uz > MAX(IC->ux, IC->uy) ||
         (IC->uz < MAX(IC->ux, IC->uy) && IC->ux != IC->uy) ||
         (IC->x != IC->px || IC->y != IC->py))
     {
        _OrientVerticesInBicomp(theGraph, R, 1);

        _OrientVerticesInEmbedding(theGraph);
        if (_RestoreAndOrientReducedPaths(theGraph, context) != OK)
            return NOTOK;

        if (_IsolateKuratowskiSubgraph(theGraph, I) != OK)
            return NOTOK;

        return NONEMBEDDABLE;
     }

/* If the Kuratowski subgraph isolator will not isolate a K3,3 based on minor E,
    then a K5 homeomorph could be isolated.  However, a K3,3 may still be tangled
    with the K5, so we now run the additional tests of the K3,3 search algorithm.

    If the search finds a K3,3 (tempResult of NONEMBEDDABLE), then we remove unwanted
    edges from the graph and return NONEMBEDDABLE.  If the search has a fault (NOTOK),
    then we return.  If the result is OK, then a K3,3 was not found at this time
    and we proceed with some clean-up work below. */

     if ((tempResult = _RunExtraK33Tests(theGraph, context)) != OK)
     {
         if (tempResult == NONEMBEDDABLE)
            if (_DeleteUnmarkedVerticesAndEdges(theGraph) != OK)
                return NOTOK;

         return tempResult;
     }

/* The extra cases for finding a K3,3 did not succeed, so the bicomp rooted by R
    is either a K5 homeomorph (with at most a superficially entangled K3,3) or
    we have made the special setting that allows us to detect the one case that
    would be too costly to try now.  Either way, we can safely reduce the bicomp
    to the 4-cycle (R, X, W, Y, R) and proceed with the planarity algorithm.
    We also restore the mixed orientation of the bicomp (i.e. the proper
    orientation in the context of the edge signs) because this code can work
    when ReduceBicomp doesn't do any actual work. */

     _OrientVerticesInBicomp(theGraph, R, 1);
     if (_ReduceBicomp(theGraph, context, R) != OK)
         return NOTOK;

/* Set visited flags to a high number so planarity algorithm
    can properly do Walkup procedure in future steps */

     _FillVisitedFlagsInBicomp(theGraph, IC->r, theGraph->N);

/* We now intend to ignore the pertinence of W (conceptually eliminating
    the connection from W to the current vertex).  Note that none of the
    bicomps listed in the pertinentBicompList (nor their respective subtrees)
    will be visited again by the planarity algorithm because they must've
    been internally active.  If they were externally active and pertinent,
    then we would've found a K3,3 by non-planarity minor B. Thus, the original
    Walkup costs that identified the pertinent bicomps we intend to ignore are
    one-time costs, preserving linear time. */

     theGraph->V[IC->w].adjacentTo = NIL;
     theGraph->V[IC->w].pertinentBicompList = NIL;

     return OK;
}

/****************************************************************************
 _RunExtraK33Tests()
 ****************************************************************************/

int  _RunExtraK33Tests(graphP theGraph, K33SearchContext *context)
{
isolatorContextP IC = &theGraph->IC;
int u_max = MAX3(IC->ux, IC->uy, IC->uz), u;

/* Case 1: If there is a pertinent or externally active vertex other than W
            on the lower external face path between X and Y (the points of
            attachment of the x-y path), then we can isolate a K3,3 homeomorph
            by Minor E1. */

    if (_SearchForMinorE1(theGraph) != OK)
        return NOTOK;

    if (IC->w != IC->z)
    {
        if (_FinishIsolatorContextInitialization(theGraph, context) != OK ||
            _IsolateMinorE1(theGraph) != OK)
            return NOTOK;

        return NONEMBEDDABLE;
    }

/* Case 2: If W/Z can make an external connection to an ancestor of V
            that is descendant to u_{max}, then a K3,3 homeomorph can
            be isolated with Minor E2.

            NOTE: It may seem costly to check the entire subtree, but
            if it succeeds then we're done, and if the only connection
            is to u_{max} then we are sure to never make this check
            again on this subtree (if all the other K3,3 tests fail).

            OPTIMIZATION: We do not check for the connection if the
            least ancestor connection from W/Z leads to an ancestor
            of u_max.  The reason is that it could ultimately be too
            costly if the connection doesn't exist, and if the highest
            numbered ancestor H of the current vertex with an external
            connection from W is a descendant u_{max} (and if no other
            tests in this function succeed), then we will discover a
            K3,3 by Minor A or B in step H.

            OPTIMIZATION: When we do test for an external connection to
            an ancestor of V descendant to u_{max}, the result may be that
            only a connection to u_{max} exists.  The result is cached
            so that the subtrees of the vertex need not be traversed again
            should we need to make the test again.
            See _SearchForDescendantExternalConnection() */

    if (IC->uz == u_max)
    {
        u = _SearchForDescendantExternalConnection(theGraph, context, IC->w, u_max);
        if (u > u_max)
        {
            IC->uz = u;
            if (_FinishIsolatorContextInitialization(theGraph, context) != OK ||
                _IsolateMinorE2(theGraph) != OK)
                return NOTOK;

            return NONEMBEDDABLE;
        }
    }

/* Case 3: If X or Y can make an external connection to an ancestor of V
            that is descendant to u_{max}, then a K3,3 homeomorph
            can be isolated with Minor E3.

            NOTE: It may seem costly to check the entire subtree, but
            if it succeeds then we're done, and if the only connection
            is to u_{max} then we are sure to never make this check
            again on this subtree (if all the other K3,3 tests fail).

            OPTIMIZATION:  Due to the prior use of the Kuratowski subgraph
            isolator, we know that at most one of X, Y or W/Z could have an
            external connection to an ancestor of u_{max} = MAX(ux, uy, uz).
            If it is X or Y, then we do not check for the lower connection
            required to find Minor E3 because it might ultimately be too
            costly.  Instead, we mark the vertex with a 'merge barrier'
            of u_{max}.  If the planar embedder attempts to merge the vertex
            prior to step u_{max}, then the embedder has found the desired
            connection and a K3,3 is isolated at that time.

            OPTIMIZATION: When we can test for an external connection to
            an ancestor of V descendant to u_{max}, the result may be that
            only a connection to u_{max} exists.  The result is cached
            so that the subtrees of the vertex need not be traversed again
            should we need to make the test again.
            See _SearchForDescendantExternalConnection() */

    if (IC->ux < u_max)
        context->V[IC->x].mergeBlocker = u_max;
    else
    {
        u = _SearchForDescendantExternalConnection(theGraph, context, IC->x, u_max);
        if (u > u_max)
        {
            IC->ux = u;
            if (_FinishIsolatorContextInitialization(theGraph, context) != OK ||
                _IsolateMinorE3(theGraph) != OK)
                return NOTOK;

            return NONEMBEDDABLE;
        }
    }

    if (IC->uy < u_max)
        context->V[IC->y].mergeBlocker = u_max;
    else
    {
        u = _SearchForDescendantExternalConnection(theGraph, context, IC->y, u_max);
        if (u > u_max)
        {
            IC->uy = u;
            if (_FinishIsolatorContextInitialization(theGraph, context) != OK ||
                _IsolateMinorE3(theGraph) != OK)
                return NOTOK;

            return NONEMBEDDABLE;
        }
    }

/* Case 4: If there exists any x-y path with points of attachment px and py
            such that px!=x or py!=y, then a K3,3 homeomorph can be isolated
            with Minor E4. */

    if (_TestForLowXYPath(theGraph) != OK)
        return NOTOK;

    if (IC->px != IC->x || IC->py != IC->y)
    {
        if (_FinishIsolatorContextInitialization(theGraph, context) != OK ||
            _IsolateMinorE4(theGraph) != OK)
            return NOTOK;

        return NONEMBEDDABLE;
    }

/* Case 5: If the x-y path contains an internal vertex that starts a second
            internal path from the internal vertex to W/Z, then a K3,3 homeomorph
            can be isolated with Minor E5. */

    if (_TestForZtoWPath(theGraph) != OK)
        return NOTOK;

    if (theGraph->G[IC->w].visited)
    {
        if (_FinishIsolatorContextInitialization(theGraph, context) != OK ||
            _IsolateMinorE5(theGraph) != OK)
            return NOTOK;

        return NONEMBEDDABLE;
    }

/* Case 6: If uz < u_{max} and there is an external connection (other than external
            connections involving X, Y and W/Z) between an ancestor of u_{max} and a
            vertex in the range [V...u_{max}), then a K3,3 homeomorph can be
            isolated with Minor E6.

            OPTIMIZATION:  See _TestForStraddlingBridge() */

    if (IC->uz < u_max)
    {
        if (_TestForStraddlingBridge(theGraph, context, u_max) != NIL)
        {
            if (_FinishIsolatorContextInitialization(theGraph, context) != OK ||
                _IsolateMinorE6(theGraph, context) != OK)
                return NOTOK;

            return NONEMBEDDABLE;
        }
    }

/* Case 7: If ux < u_{max} or uy < u_{max} and there is an external connection
            between an ancestor of u_{max} and a vertex in the range [V...u_{max})
            (except for external connections involving X, Y and W/Z), then a K3,3
            homeomorph can be isolated with Minor E7.

            OPTIMIZATION: Same as Case 6.*/

    if (IC->ux < u_max || IC->uy < u_max)
    {
        if (_TestForStraddlingBridge(theGraph, context, u_max) != NIL)
        {
            if (_FinishIsolatorContextInitialization(theGraph, context) != OK ||
                _IsolateMinorE7(theGraph, context) != OK)
                return NOTOK;

            return NONEMBEDDABLE;
        }
    }

/* If none of the tests found a K3,3, then we return OK to indicate that nothing
    went wrong, but a K3,3 was not found. */

    return OK;
}

/****************************************************************************
 _SearchForMinorE1()
 Search along the external face below the x-y path for a vertex Z other
 than W that is externally active or pertinent.
 ****************************************************************************/

int _SearchForMinorE1(graphP theGraph)
{
int  Z=theGraph->IC.px, ZPrevLink=1;

     Z = _GetNextVertexOnExternalFace(theGraph, Z, &ZPrevLink);

     while (Z != theGraph->IC.py)
     {
         if (Z != theGraph->IC.w)
         {
            if (_VertexActiveStatus(theGraph, Z, theGraph->IC.v) == VAS_EXTERNAL)
            {
                theGraph->IC.z = Z;
                theGraph->IC.uz = _GetLeastAncestorConnection(theGraph, Z);
                return OK;
            }
            else if (theGraph->V[Z].pertinentBicompList != NIL ||
                     theGraph->V[Z].adjacentTo == theGraph->IC.v)
            {
                /* Swap the roles of W and Z */

                theGraph->IC.z = theGraph->IC.w;
                theGraph->IC.w = Z;

                /* If the new W (indicated by Z) was on the path (R, X, old W) then
                    the new Z (the old W, which has no type mark) is on the path
                    (X, new W, new Z, Y) so we change the type new Z to being on the
                    RYW path. Otherwise, the order is (X, new Z, new W, Y), so the
                    new Z (old W with no type) is type changed to be on the RXW path.*/

                if (theGraph->G[Z].type == VERTEX_LOW_RXW)
                     theGraph->G[theGraph->IC.z].type = VERTEX_LOW_RYW;
                else theGraph->G[theGraph->IC.z].type = VERTEX_LOW_RXW;

                /* For completeness, we change the new W to type unknown */

                theGraph->G[theGraph->IC.w].type = TYPE_UNKNOWN;

                /* The external activity ancestor connection of the new Z must be obtained */

                theGraph->IC.uz = _GetLeastAncestorConnection(theGraph, theGraph->IC.z);

                return OK;
            }
         }

         Z = _GetNextVertexOnExternalFace(theGraph, Z, &ZPrevLink);
     }

     return OK;
}

/****************************************************************************
 _FinishIsolatorContextInitialization()
 Once it has been decided that a desired subgraph can be isolated, it
 becomes safe to finish the isolator context initialization.
 ****************************************************************************/

int  _FinishIsolatorContextInitialization(graphP theGraph, K33SearchContext *context)
{
isolatorContextP IC = &theGraph->IC;

/* Restore the orientation of the bicomp on which we're working, then
    perform orientation of all vertices in graph. (An unnecessary but
    polite step that simplifies the description of key states of the
    data structures). */

     _OrientVerticesInBicomp(theGraph, IC->r, 1);
     _OrientVerticesInEmbedding(theGraph);

/* Restore any paths that were reduced to single edges */

     if (_RestoreAndOrientReducedPaths(theGraph, context) != OK)
         return NOTOK;

/* We assume that the current bicomp has been marked appropriately,
     but we must now clear the visitation flags of all other bicomps. */

     _FillVisitedFlagsInOtherBicomps(theGraph, IC->r, 0);

/* To complete the normal behavior of _FillVisitedFlags() in the
    normal isolator context initialization, we also have to clear
    the visited flags on all edges that have not yet been embedded */

     _FillVisitedFlagsInUnembeddedEdges(theGraph, 0);

/* Now we can find the descendant ends of unembedded back edges based on
     the ancestor settings ux, uy and uz. */

     if (_FindExternalConnectionDescendantEndpoint(theGraph, IC->ux, IC->x, &IC->dx) != OK ||
         _FindExternalConnectionDescendantEndpoint(theGraph, IC->uy, IC->y, &IC->dy) != OK ||
         _FindExternalConnectionDescendantEndpoint(theGraph, IC->uz, IC->z, &IC->dz) != OK)
         return NOTOK;

/* Finally, we obtain the descendant end of an unembedded back edge to
     the current vertex. */

     if (_FindUnembeddedEdgeToCurVertex(theGraph, IC->w, &IC->dw) != OK)
         return NOTOK;

     return OK;
}

/****************************************************************************
 _GetAdjacentAncestorInRange()
 Returns the ancestor of theVertex that is adjacent to theVertex by an
 unembedded back edge and has a DFI strictly between closerAncestor and
 fartherAncestor.
 Returns NIL if theVertex has no such neighboring ancestor.
 ****************************************************************************/

int _GetAdjacentAncestorInRange(graphP theGraph, K33SearchContext *context, int theVertex,
                                int closerAncestor, int fartherAncestor)
{
int J = context->V[theVertex].backArcList;

    while (gp_IsEdge(theGraph, J))
    {
        if (theGraph->G[J].v < closerAncestor &&
            theGraph->G[J].v > fartherAncestor)
            return theGraph->G[J].v;

        J = gp_GetNextEdge(theGraph, J);
        if (J == context->V[theVertex].backArcList)
            J = NIL;
    }
    return NIL;
}

/****************************************************************************
 _SearchForDescendantExternalConnection()
 Search the cutVertex and each subtree rooted by a vertex in the
 separatedDFSChildList of the cut vertex for an external connection
 to a vertex ancestor to the current vertex V and descendant to u_max.

 The function returns the descendant of u_max found to have an external
 connection to the given cut vertex.

 OPTIMIZATION: The subtrees are processed by preorder visitation.  If
 a vertex is visited and has a lowpoint indicating that it and its
 descendants make no external connections, then we prune the subtree,
 eliminating the vertex and its descendants from consideration.
 Otherwise, if the vertex has an externalConnectionAncestor setting,
 which must have been made by a prior invocation of this function,
 then we use that setting.  If both of these tests fail, then
 we examine the vertex and push its children for consideration.
 This ensures that this procedure never explores a subtree more than
 once during the life of the K3,3 search algorithm.

 Note that if a subtree is explored and the desired external connection
 descendant to u_{max} is found, then a K3,3 will be found, so we only
 have to concern ourselves with subtrees that connect only to u_{max}.
 Between steps v and u_{max}, the subtree search is optimized by setting
 'externalConnectionAncestor', and steps after u_{max} process ancestors
 of u_{max}.  Since this routine is only called if the cutVertex makes
 no external connections to ancestors of u_{max}, the lowpoint test
 prevents this subtree from being searched after step u_{max}.
 ****************************************************************************/

int  _SearchForDescendantExternalConnection(graphP theGraph, K33SearchContext *context, int cutVertex, int u_max)
{
isolatorContextP IC = &theGraph->IC;
int  u2 = _GetAdjacentAncestorInRange(theGraph, context, cutVertex, IC->v, u_max);
int  listHead, child, descendant;

/* Test cut vertex for external connection to descendant of u_max */

     if (u2 != NIL)
         return u2;

/* If there is no direct back edge connection from the cut vertex
        to a vertex on the path between V and u_max, then we will
        look for such a connection in the DFS subtrees rooted by
        separated DFS children of the vertex (ignoring those whose
        lowpoint indicates that they make no external connections) */

     /* Begin by pushing the separated DFS children of the cut vertex,
        except stop when the lowpoint is no longer less than V since
        external connections are not being made. */

     sp_ClearStack(theGraph->theStack);
     listHead = child = theGraph->V[cutVertex].separatedDFSChildList;
     while (child != NIL)
     {
         if (theGraph->V[child].Lowpoint >= IC->v)
             break;
         sp_Push(theGraph->theStack, child);
         child = LCGetNext(theGraph->DFSChildLists, listHead, child);
     }

     /* Now process the stack until it is empty or until we've found the desired connection. */

     while (!sp_IsEmpty(theGraph->theStack))
     {
         sp_Pop(theGraph->theStack, descendant);

         /* If the vertex has a lowpoint indicating that it makes no external
            connections, then skip the subtree rooted by the vertex */

         if (theGraph->V[descendant].Lowpoint < IC->v)
         {
             /* If a prior invocation has precalculated the result, use it. */

             if (context->V[descendant].externalConnectionAncestor != NIL)
             {
                 /* If the result is in the range we need, return it.  Otherwise,
                    skip the subtree rooted by the vertex. */

                 if (context->V[descendant].externalConnectionAncestor < IC->v &&
                     context->V[descendant].externalConnectionAncestor > u_max)
                     return context->V[descendant].externalConnectionAncestor;
             }

             /* If the subtree has not been explored, then explore it. */

             else
             {
                 /* Check the subtree root for the desired connection. */

                 u2 = _GetAdjacentAncestorInRange(theGraph, context, descendant, IC->v, u_max);
                 if (u2 != NIL)
                     return u2;

                 /* Push each child as a new subtree root to be considered,
                    except skip those whose lowpoint is too great. */

                 child = context->V[descendant].sortedDFSChildList;
                 while (child != NIL)
                 {
                     if (theGraph->V[child].Lowpoint < IC->v)
                         sp_Push(theGraph->theStack, child);

                     child = LCGetNext(context->sortedDFSChildLists,
                                       context->V[descendant].sortedDFSChildList, child);
                 }
             }
         }
     }

/* The only external connections from the cutVertex lead to u_max,
    so cache the result and return it. */

     context->V[cutVertex].externalConnectionAncestor = u_max;
     return u_max;
}

/****************************************************************************
 _FindExternalConnectionDescendantEndpoint()
 Returns OK if it finds that either the given cutVertex or one of its
    descendants in a separated bicomp has an unembedded back edge
    connection to the given ancestor vertex.
 Returns NOTOK otherwise (it is an error to not find the descendant because
    this function is only called if _SearchForDescendantExternalConnection()
    has already determined the existence of the descendant).
 ****************************************************************************/

int  _FindExternalConnectionDescendantEndpoint(graphP theGraph, int ancestor,
                                               int cutVertex, int *pDescendant)
{
int  listHead, child, J;

     // Check whether the cutVertex is directly adjacent to the ancestor
     // by an unembedded back edge.

     J = theGraph->V[ancestor].fwdArcList;
     while (gp_IsEdge(theGraph, J))
     {
         if (theGraph->G[J].v == cutVertex)
         {
             *pDescendant = cutVertex;
             return OK;
         }

         J = gp_GetNextEdge(theGraph, J);
         if (J == theGraph->V[ancestor].fwdArcList)
             J = NIL;
     }

     // Now check the descendants of the cut vertex to see if any make
     // a connection to the ancestor.
     listHead = child = theGraph->V[cutVertex].separatedDFSChildList;
     while (child != NIL)
     {
         if (theGraph->V[child].Lowpoint >= theGraph->IC.v)
             break;

         if (_FindUnembeddedEdgeToSubtree(theGraph, ancestor, child, pDescendant) == OK)
             return OK;

         child = LCGetNext(theGraph->DFSChildLists, listHead, child);
     }

     return NOTOK;
}

/****************************************************************************
 _SearchForMergeBlocker()

 This function helps to implement the merge blocking optimization of
 _SearchForDescendantExternalConnection().  The function RunExtraK33Tests()
 sets a mergeBlocker rather than run _SearchForDescendantExternalConnection()
 in certain cases.  This procedure is called by MergeBicomps to test the
 embedding stack for a merge blocker before merging any biconnected components.
 If a merge blocker is found, then the embedder's Walkdown function is
 terminated and SearchForK33() is subsequently called.  The blocked merge
 point is then used as the basis for isolating a K3,3.

 Returns OK on success (whether or not the search found a merge blocker)
         NOTOK on internal function failure
         pMergeBlocker is set to NIL unless a merge blocker is found.
 ****************************************************************************/

int  _SearchForMergeBlocker(graphP theGraph, K33SearchContext *context, int I, int *pMergeBlocker)
{
stackP tempStack;
int  R, Rout, Z, ZPrevLink;

/* Set return result to 'not found' then return if there is no stack to inspect */

     *pMergeBlocker = NIL;

     if (sp_IsEmpty(theGraph->theStack))
         return OK;

/* Create a copy of the embedding stack */

     tempStack = sp_Duplicate(theGraph->theStack);
     if (tempStack == NULL)
         return NOTOK;

/* Search the copy of the embedding stack for a merge blocked vertex */

     while (!sp_IsEmpty(tempStack))
     {
         sp_Pop2(tempStack, R, Rout);
         sp_Pop2(tempStack, Z, ZPrevLink);

         if (context->V[Z].mergeBlocker != NIL &&
             context->V[Z].mergeBlocker < I)
         {
             *pMergeBlocker = Z;
             break;
         }
     }

     sp_Free(&tempStack);
     return OK;
}

/****************************************************************************
 _FindK33WithMergeBlocker()

 This function completes the merge blocking optimization by isolating a K3,3
 based on minor E3 if a merge blocked vertex was previously found.

 Returns OK on success, NOTOK on internal function failure
 ****************************************************************************/

int  _FindK33WithMergeBlocker(graphP theGraph, K33SearchContext *context, int I, int mergeBlocker)
{
int  R, RPrevLink, u_max, u, J, W;
isolatorContextP IC = &theGraph->IC;

/* First, we orient the vertices so we can successfully restore all of the
    reduced paths.  This needs to be done before reconstructing the context
    for CASE 3 of RunExtraK33Tests() because the reconstruction involves
    using the Walkup to I from a descendant of I, which will not work if
    the descendant is in one of the reduced paths. */

     _OrientVerticesInEmbedding(theGraph);
     if (_RestoreAndOrientReducedPaths(theGraph, context) != OK)
         return NOTOK;

/* Reconstruct the context that was present for CASE 3 of RunExtraK33Tests()
        when we decided to set a mergeBlocker rather than calling
        _SearchForDescendantExternalConnection() */

     /* Obtain the root of the bicomp containing the mergeBlocker. */

     RPrevLink = 1;
     R = mergeBlocker;
     while (R < theGraph->N)
        R = _GetNextVertexOnExternalFace(theGraph, R, &RPrevLink);

     /* Switch the 'current step' variable I to be equal to the
       non-virtual counterpart of the bicomp root. */

     I = theGraph->V[R - theGraph->N].DFSParent;

     /* Eliminate the visitation and pertinence settings for step u_max */

     _FillVisitedFlags(theGraph, I+1);

     for (J = 0; J < theGraph->N; J++)
     {
         theGraph->V[J].adjacentTo = NIL;
         theGraph->V[J].pertinentBicompList = NIL;
     }

     /* Restore the pertinence settings of step I by doing the Walkup for each
        back edge that was not embedded when step I was originally performed. */

     J = theGraph->V[I].fwdArcList;
     while (gp_IsEdge(theGraph, J))
     {
        W = theGraph->G[J].v;
        _WalkUp(theGraph, I, W);

        J = gp_GetNextEdge(theGraph, J);
        if (J == theGraph->V[I].fwdArcList)
            J = NIL;
     }

/* Next, we make the standard initialization calls for when we have found
     a non-planarity condition. */

     sp_ClearStack(theGraph->theStack);

     if (_ChooseTypeOfNonplanarityMinor(theGraph, I, R) != OK)
         return NOTOK;

     IC->ux = _GetLeastAncestorConnection(theGraph, IC->x);
     IC->uy = _GetLeastAncestorConnection(theGraph, IC->y);
     IC->uz = _GetLeastAncestorConnection(theGraph, IC->z);

     u_max = MAX3(IC->ux,IC->uy,IC->uz);

/* Perform the remainder of CASE 3 of RunExtraK33Tests() */

     if (mergeBlocker == IC->x)
     {
         u = _SearchForDescendantExternalConnection(theGraph, context, IC->x, u_max);
         if (u > u_max)
         {
             IC->ux = u;
             if (_FinishIsolatorContextInitialization(theGraph, context) != OK ||
                 _IsolateMinorE3(theGraph) != OK)
                 return NOTOK;
         }
         else return NOTOK;
     }
     else if (mergeBlocker == IC->y)
     {
         u = _SearchForDescendantExternalConnection(theGraph, context, IC->y, u_max);
         if (u > u_max)
         {
             IC->uy = u;
             if (_FinishIsolatorContextInitialization(theGraph, context) != OK ||
                 _IsolateMinorE3(theGraph) != OK)
                 return NOTOK;
         }
         else return NOTOK;
     }
     else return NOTOK;

/* Do the final clean-up to obtain the K3,3 */

     if (_DeleteUnmarkedVerticesAndEdges(theGraph) != OK ||
         _RestoreAndOrientReducedPaths(theGraph, context) != OK)
         return NOTOK;

     return OK;
}

/****************************************************************************
 _TestForLowXYPath()
 Is there an x-y path that does not include X?
 If not, is there an x-y path that does not include Y?
 If not, then we restore the original x-y path.
 If such a low x-y path exists, then we adjust px or py accordingly,
    and we make sure that X or Y (whichever is excluded) and its edges are
    not marked visited.
 ****************************************************************************/

int  _TestForLowXYPath(graphP theGraph)
{
isolatorContextP IC = &theGraph->IC;
stackP realStack=theGraph->theStack, auxStack=NULL;
int  result;

/* Clear the previously marked X-Y path */

     _FillVisitedFlagsInBicomp(theGraph, IC->r, 0);

/* Create a stack and use it to hide the internal edges of X */

     auxStack = sp_New(gp_GetVertexDegree(theGraph, IC->x));
     if (auxStack == NULL) return NOTOK;
     theGraph->theStack = auxStack;
     _HideInternalEdges(theGraph, IC->x);
     theGraph->theStack = realStack;

/* Try to find a low X-Y path that excludes X, then restore the
    internal edges of X. */

     result = _MarkHighestXYPath(theGraph);
     theGraph->theStack = auxStack;
     _RestoreInternalEdges(theGraph);
     theGraph->theStack = realStack;
     sp_Free(&auxStack);

/* If we found the low X-Y path, then return. */

     if (result == OK)
         return OK;

/* Create a stack and use it to hide the internal edges of Y */

     auxStack = sp_New(gp_GetVertexDegree(theGraph, IC->y));
     if (auxStack == NULL) return NOTOK;
     theGraph->theStack = auxStack;
     _HideInternalEdges(theGraph, IC->y);
     theGraph->theStack = realStack;

/* Try to find a low X-Y path that excludes Y, then restore the
    internal edges of Y. */

     result = _MarkHighestXYPath(theGraph);
     theGraph->theStack = auxStack;
     _RestoreInternalEdges(theGraph);
     theGraph->theStack = realStack;
     sp_Free(&auxStack);

/* If we found the low X-Y path, then return. */

     if (result == OK)
         return OK;

/* Restore the original X-Y path and return with no error
        (the search failure is reflected by no change to px and py */

     _MarkHighestXYPath(theGraph);
     return OK;
}

/****************************************************************************
 _TestForZtoWPath()
 This function tests whether there is a path inside the bicomp leading from W
 to some internal node of the x-y path.  If there is, the path is marked.

 Upon function return, the marking of W distinguishes whether the path was found.
 The function returns NOTOK on internal error, OK otherwise.

 All internal vertices are marked as type unknown, as are W and the bicomp
 root.  There is an X-Y path marked visited.  So, we start a depth first
 search from W to find a visited vertex, except we prune the search to
 ignore vertices whose type is not unknown.

 The depth first search has to mark the vertices it has seen as visited,
 but we do not want to conflict with the visited/non-visited settings
 that have so far been used to isolate the X-Y path.  So, each vertex
 visited is marked with a NIL and pushed onto the resetList.  At the end,
 all vertices on the resetList have their visited flags reset to 0.

 For each vertex we visit, if it is an internal vertex on the X-Y path
 (i.e. visited=1 and type unknown), then we want to stop and unroll the
 stack to obtain the desired path (described below). If the vertex is type
 unknown, then we want to visit its unvisited neighbors.

 We want to manage the stack so that it when the desired vertex is found,
 the stack contains the desired path.  So, we do not simply push the
 neighbors of the vertex being visited.  First, we only push 'eligible'
 vertices (i.e. vertices with a type of unknown and visited not equal to
 NIL).  Second, when we decide a vertex v is eligible, we push (v, NIL).
 When we pop (v, NIL), we know that its type is unknown so we test
 whether it is the desired vertex by checking if its visited member is
 equal to 1.  If so, then we can stop the depth first search, process
 the resetList, then use the vertices and edges remaining on the
 stack to mark the desired path.

 If we pop (v, NIL) and find that the visited of v equals 0, then we
 set its visited to NIL.  Then we find the first edge record e leading
 to an eligible vertex w (i.e. a vertex with type unknown and visited
 not equal to NIL), and we push both (v, e) and (w, NIL).  Eventually all
 paths leading from w will be explored, and if none find the desired vertex,
 then (v, e) is popped.  Now we search the adjacency list of v starting
 after e to find the edge record that indicates the next eligible vertex
 to visit.  If none are found, then we simply go to the next iteration,
 which pops a 2-tuple containing the vertex u and an edge record e that
 indicates v as the neighbor of u.  Finally, if the stack empties without
 finding the desired vertex, then we simply process the resetStack and return.
 ****************************************************************************/

int  _TestForZtoWPath(graphP theGraph)
{
isolatorContextP IC = &theGraph->IC;
stackP resetList = sp_New(_GetBicompSize(theGraph, IC->r));
int  v, e, w;

     if (resetList == NULL) return NOTOK;

     sp_ClearStack(theGraph->theStack);
     sp_Push2(theGraph->theStack, IC->w, NIL);

     while (!sp_IsEmpty(theGraph->theStack))
     {
          sp_Pop2(theGraph->theStack, v, e);

          if (e == NIL)
          {
              if (theGraph->G[v].visited)
                  break;

              theGraph->G[v].visited = NIL;
              sp_Push(resetList, v);

              e = gp_GetFirstEdge(theGraph, v);
          }
          else
              e = gp_GetNextEdge(theGraph, e);

          while (gp_IsEdge(theGraph, e))
          {
              w = theGraph->G[e].v;
              if (theGraph->G[w].visited != NIL &&
                  theGraph->G[w].type == TYPE_UNKNOWN)
              {
                  sp_Push2(theGraph->theStack, v, e);
                  sp_Push2(theGraph->theStack, w, NIL);
                  break;
              }
              e = gp_GetNextEdge(theGraph, e);
          }
     }

     while (!sp_IsEmpty(resetList))
     {
         sp_Pop(resetList, v);
         theGraph->G[v].visited = 0;
     }
     sp_Free(&resetList);

     while (!sp_IsEmpty(theGraph->theStack))
     {
         sp_Pop2(theGraph->theStack, v, e);
         theGraph->G[v].visited = 1;
         theGraph->G[e].visited = 1;
         theGraph->G[gp_GetTwinArc(theGraph, e)].visited = 1;
     }

     return OK;
}

/****************************************************************************
 _TestForStraddlingBridge()
 We proceed on the path [V...u_{max}) from the current vertex V up to and
 excluding u_{max}.  For each vertex p, we test whether p has a least
 ancestor less than u_{max} and whether p has a DFS child c that is not an
 ancestor of X, Y and W and that has a connection to an ancestor of u_{max}
 (in other words, whether the child C has a lowpoint less than u_{max}).

 The separatedDFSChildList of each vertex already contains a list of
 the DFS children sorted by their lowpoint, and the list has not been
 reduced by bicomp merging because the vertices are not descendants of V.
 So, we can process a vertex by examining its leastAncestor and the
 lowpoint of one of the first two elements in its separatedDFSChildList.
 If the first child is an ancestor of X, Y and W, then we look at the
 second child.

 If no bridge straddling u_{max} is found, the function returns NIL.
 If a straddling bridge is found, the function returns a descendant d
 of p in the subtree rooted by c such that d has a leastAncestor less
 than u_{max}.  Given the vertex d, the path through the straddling
 bridge required in Minors E6 and E7 is easy to identify:  Mark the
 DFS tree path from d to p, and add and mark the edge from d to its
 least ancestor.

 OPTIMIZATION: If a straddling bridge is not found, then in each tree edge of
        the path [V...u_{max}) we set the member noStraddle equal to u_{max}.
        Then, we modify the above stated routine so that if it is testing
        for a straddling bridge of u_{max} along this path, it will stop
        if it encounters an edge with noStraddle equal to u_{max} then it
        will stop.  Also, the optimization will only set noStraddle equal to
        u_{max} on the portion of the path that is traversed.  Finally, if
        noStraddle is set to a value other than NIL, the setting will be
        ignored and it will not be changed.

        Due to this optimization, we do not traverse a path more than once
        to find out whether a vertex on the path has a bridge that straddles
        u_{max}.  This leaves two questions:
            1) What if a future step must determine whether there is a
                straddling bridge of an ancestor of u_{max}?
            2) What if a future step must determine whether there is a
                straddling bridge of a descendant of u_{max}?

        The condition described in the first question cannot occur because it
        would imply the ability to detect a straddling bridge now.
        The condition described by the second question may occur, but in the
        future step, the bicomp now being tested for a K3,3 will be part of
        a straddling bridge in that future step.  Thus, the straddling
        bridge query is asked at most twice along any DFS tree path.
 ****************************************************************************/

int  _TestForStraddlingBridge(graphP theGraph, K33SearchContext *context, int u_max)
{
isolatorContextP IC = &theGraph->IC;
int  p, c, d, excludedChild, e;

     p = IC->v;
     excludedChild = IC->r - theGraph->N;
     d = NIL;

/* Starting at V, traverse the ancestor path to u_max looking for a straddling bridge */

     while (p > u_max)
     {
         /* If we find a direct edge from p to an ancestor of u_max, the break. */

         if (theGraph->V[p].leastAncestor < u_max)
         {
             d = p;
             break;
         }

         /* Check for a path from p to an ancestor of u_max using the child
            of p with the least Lowpoint, except the child that is an
            ancestor of X, Y and W. */

         c = theGraph->V[p].separatedDFSChildList;
         if (c == excludedChild)
             c = LCGetNext(theGraph->DFSChildLists, c, c);

         if (c != NIL && theGraph->V[c].Lowpoint < u_max)
         {
             _FindUnembeddedEdgeToSubtree(theGraph, theGraph->V[c].Lowpoint, c, &d);
             break;
         }

         /* Check for noStraddle of u_max, break if found */

         e = gp_GetFirstEdge(theGraph, p);
         if (context->G[e].noStraddle == u_max)
             break;

         /* Go to the next ancestor */

         excludedChild = p;
         p = theGraph->V[p].DFSParent;
     }

/* If d is NIL, then no straddling bridge was found, so we do the
        noStraddle optimization. */

     if (d == NIL)
     {
         c = IC->v;
         while (c != p)
         {
             e = gp_GetFirstEdge(theGraph, c);
             if (context->G[e].noStraddle != NIL)
                 break;

             context->G[e].noStraddle = u_max;

             c = theGraph->V[c].DFSParent;
         }
     }

/* Return either NIL indicating no bridge straddling u_max or the descendant d
         used to help mark a straddling bridge that was found by this test. */

     return d;
}

/****************************************************************************
 _ReduceBicomp()

 We want to reduce the given biconnected component to a 4-cycle plus an
 internal edge connecting X and Y.  Each edge is to be associated with a
 path from the original graph, preserving the depth first search tree
 paths that help connect the vertices R, X, Y, and W.  If a K3,3 is later found,
 the paths are restored, but it is necessary to preserve the DFS tree so that
 functions like MarkDFSPath() will be able to pass through the restored bicomp.
 Also, if a K3,3 later found due to the merge blocker optimization, then the
 internal X-Y path may be needed and, once the bicomp reduction is reversed,
 a full DFS subtree connecting all vertices in the bicomp will need to be
 restored or else functions that traverse the bicomp will not work.

 For example, _FindK33WithMergeBlocker() invokes ChooseTypeOfNonplanarityMinor()
 to help reconstruct the context under which the mergeBlocker was set.
 ChooseTypeOfNonplanarityMinor() calls _FillVisitedFlagsInBicomp(), which
 depends on the DFS tree.
 ****************************************************************************/

int  _ReduceBicomp(graphP theGraph, K33SearchContext *context, int R)
{
isolatorContextP IC = &theGraph->IC;
int  min, mid, max, A, A_edge, B, B_edge;
int  rxType, xwType, wyType, yrType, xyType;

/* The vertices in the bicomp need to be oriented so that functions
    like MarkPathAlongBicompExtFace() will work. */

     _OrientVerticesInBicomp(theGraph, R, 0);

/* The reduced edges start with a default type of 'tree' edge. The
     tests below, which identify the additional non-tree paths
     needed to complete the reduced bicomp, also identify which
     reduced edges need to be cycle edges.*/

     rxType = xwType = wyType = yrType = xyType = EDGE_DFSPARENT;

/* Now we calculate some values that help figure out the shape of the
    DFS subtree whose structure will be retained in the bicomp. */

     min = MIN3(IC->x, IC->y, IC->w);
     max = MAX3(IC->x, IC->y, IC->w);
     mid = MAX3(MIN(IC->x, IC->y), MIN(IC->x, IC->w), MIN(IC->y, IC->w));

/* If the order of descendendancy from V goes first to X, then it can
    proceed either to W then Y or to Y then W */

     if (min == IC->x)
     {
         /* A is a descendant adjacent to the current vertex by a cycle edge
            whose DFS tree path to either mid or max is combined with the
            cycle edge to form the path that will be reduced to the
            external face cycle edge (V, max). */

         A_edge = gp_GetLastEdge(theGraph, IC->r);
         A = theGraph->G[A_edge].v;
         yrType = EDGE_BACK;

         /* If Y is max, then a path parallel to the X-Y path will be a
            second path reduced to a cycle edge.  We find the neighbor B
            of min=X on the X-Y path.  The edge (B, min) is a cycle edge
            that, along with the DFS tree path (B, ..., max), will be
            retained and reduced to a cycle edge. */

         if (max == IC->y)
         {
             B_edge = gp_GetLastEdge(theGraph, IC->x);
             while (B_edge != gp_GetFirstEdge(theGraph, IC->x))
             {
                 if (theGraph->G[B_edge].visited) break;
                 B_edge = gp_GetPrevEdge(theGraph, B_edge);
             }

             if (!theGraph->G[B_edge].visited)
                 return NOTOK;

             B = theGraph->G[B_edge].v;
             xyType = EDGE_BACK;
         }

         /* Otherwise, W is max so we find the neighbor B of min=X on the
            lower external face path (X, ..., W), which excludes V.  The
            cycle edge (B, min) and the DFS tree path (B, max) will be
            retained and reduced to a cycle edge.*/

         else if (max == IC->w)
         {
             B_edge = gp_GetFirstEdge(theGraph, IC->x);
             B = theGraph->G[B_edge].v;
             xwType = EDGE_BACK;
         }

         else return NOTOK;
     }

/* Otherwise, the order of descendancy from V goes first to Y, then it
     proceeds to either W then X or to X then W. The */

     else
     {
         A_edge = gp_GetFirstEdge(theGraph, IC->r);
         A = theGraph->G[A_edge].v;
         rxType = EDGE_BACK;

         if (max == IC->x)
         {
             B_edge = gp_GetFirstEdge(theGraph, IC->y);
             while (B_edge != gp_GetLastEdge(theGraph, IC->y))
             {
                 if (theGraph->G[B_edge].visited) break;
                 B_edge = gp_GetNextEdge(theGraph, B_edge);
             }

             if (!theGraph->G[B_edge].visited)
                 return NOTOK;

             B = theGraph->G[B_edge].v;
             xyType = EDGE_BACK;
         }

         else if (max == IC->w)
         {
             B_edge = gp_GetLastEdge(theGraph, IC->y);
             B = theGraph->G[B_edge].v;
             wyType = EDGE_BACK;
         }

         else return NOTOK;
     }

/* Now that we have collected the information on which cycle edge and
    which tree paths will actually be retained, we clear the visited
    flags so the current X-Y path will not be retained (an X-Y path
    formed mostly or entirely from DFS tree edges is retained). */

     _FillVisitedFlagsInBicomp(theGraph, R, 0);

/* Now we mark the tree path from the maximum numbered vertex up
      to the bicomp root. This marks one of the following four paths:
      Case 1. (V, ..., X=min, ..., W=mid, ..., Y=max)
      Case 2. (V, ..., X=min, ..., Y=mid, ..., W=max)
      Case 3. (V, ..., Y=min, ..., W=mid, ..., X=max)
      Case 4. (V, ..., Y=min, ..., X=mid, ..., W=max) */

     if (theGraph->functions.fpMarkDFSPath(theGraph, R, max) != OK)
         return NOTOK;

/* Now we use A to mark a path on the external face corresponding to:
      Case 1. (V, ..., Y=max)
      Case 2. (V, ..., Y=mid)
      Case 3. (V, ..., X=max)
      Case 4. (V, ..., X=mid) */

     if (theGraph->functions.fpMarkDFSPath(theGraph, min==IC->x ? IC->y : IC->x, A) != OK)
         return NOTOK;

     theGraph->G[A_edge].visited = 1;
     theGraph->G[gp_GetTwinArc(theGraph, A_edge)].visited = 1;

/* Now we use B to mark either an X-Y path or a path of the external face
      corresponding to:
      Case 1. (X=min, ..., B, ..., Y=max)
      Case 2. (X=min, ..., B, ..., W=max)
      Case 3. (Y=min, ..., B, ..., X=max)
      Case 4. (Y=min, ..., B, ..., W=max) */

     if (theGraph->functions.fpMarkDFSPath(theGraph, max, B) != OK)
         return NOTOK;

     theGraph->G[B_edge].visited = 1;
     theGraph->G[gp_GetTwinArc(theGraph, B_edge)].visited = 1;

/* Delete the unmarked edges in the bicomp. */

     _DeleteUnmarkedEdgesInBicomp(theGraph, R);

/* Clear all visited flags and orientation signs in the bicomp.
     Note that the bicomp may not be properly oriented at this point
     because we may have exchanged external face paths for internal
     DFS tree paths.  However, the reduced bicomp will be properly
     oriented, and the paths of degree 2 vertices will have their
     orientations fixed if/when reduction edges are restored. */

     _FillVisitedFlagsInBicomp(theGraph, R, 0);
     _ClearInvertedFlagsInBicomp(theGraph, R);

/* Reduce the paths to single edges. */

     if (_ReduceExternalFacePathToEdge(theGraph, context, R, IC->x, rxType) != OK ||
         _ReduceExternalFacePathToEdge(theGraph, context, IC->x, IC->w, xwType) != OK ||
         _ReduceExternalFacePathToEdge(theGraph, context, IC->w, IC->y, wyType) != OK ||
         _ReduceExternalFacePathToEdge(theGraph, context, IC->y, R, yrType) != OK)
         return NOTOK;

     if (_ReduceXYPathToEdge(theGraph, context, IC->x, IC->y, xyType) != OK)
         return NOTOK;

     return OK;
}

/****************************************************************************
 _ComputeArcType()
 This is just a little helper function that automates a sequence of decisions
 that has to be made a number of times.
 An edge record is being added to the adjacency list of a; it indicates that
 b is a neighbor.  The edgeType can be either 'tree' (EDGE_DFSPARENT) or
 'cycle' (EDGE_BACK).  If a or b is a root copy, we translate to the
 non-virtual counterpart, then determine which has the lesser DFI.  If a
 has the lower DFI then the edge record is a tree edge to a child
 (EDGE_DFSCHILD) if edgeType indicates a tree edge.  If edgeType indicates a
 cycle edge, then it is a forward cycle edge (EDGE_FORWARD) to a descendant.
 Symmetric conditions define the types for a > b.
 ****************************************************************************/

int  _ComputeArcType(graphP theGraph, int a, int b, int edgeType)
{
     if (a >= theGraph->N)
         a = theGraph->V[a - theGraph->N].DFSParent;
     if (b >= theGraph->N)
         b = theGraph->V[b - theGraph->N].DFSParent;

     if (a < b)
         return edgeType == EDGE_DFSPARENT ? EDGE_DFSCHILD : EDGE_FORWARD;

     return edgeType == EDGE_DFSPARENT ? EDGE_DFSPARENT : EDGE_BACK;
}

/****************************************************************************
 _ReduceExternalFacePathToEdge()
 ****************************************************************************/

int  _ReduceExternalFacePathToEdge(graphP theGraph, K33SearchContext *context, int u, int x, int edgeType)
{
int  prevLink, v, w, e;

     /* If the path is a single edge, then no need for a reduction */

     prevLink = 1;
     v = _GetNextVertexOnExternalFace(theGraph, u, &prevLink);
     if (v == x)
         return OK;

     /* We have the endpoints u and x of the path, and we just computed the
        first vertex internal to the path and a neighbor of u.  Now we
        compute the vertex internal to the path and a neighbor of x. */

     prevLink = 0;
     w = _GetNextVertexOnExternalFace(theGraph, x, &prevLink);

     /* Delete the two edges that connect the path to the bicomp.
        If either edge is a reduction edge, then we have to restore
        the path it represents. We can only afford to visit the
        endpoints of the path.
        Note that in the restored path, the edge incident to each
        endpoint of the original path is a newly added edge,
        not a reduction edge. */

     e = gp_GetFirstEdge(theGraph, u);
     if (context->G[e].pathConnector != NIL)
     {
         if (_RestoreReducedPath(theGraph, context, e) != OK)
             return NOTOK;
         e = gp_GetFirstEdge(theGraph, u);
         v = theGraph->G[e].v;
     }
     gp_DeleteEdge(theGraph, e, 0);

     e = gp_GetLastEdge(theGraph, x);
     if (context->G[e].pathConnector != NIL)
     {
         if (_RestoreReducedPath(theGraph, context, e) != OK)
             return NOTOK;
         e = gp_GetLastEdge(theGraph, x);
         w = theGraph->G[e].v;
     }
     gp_DeleteEdge(theGraph, e, 0);

     /* Add the reduction edge, then set its path connectors so the original
        path can be recovered and set the edge type so the essential structure
        of the DFS tree can be maintained (The 'Do X to Bicomp' functions
        and functions like MarkDFSPath(0 depend on this). */

     gp_AddEdge(theGraph, u, 0, x, 1);

     e = gp_GetFirstEdge(theGraph, u);
     context->G[e].pathConnector = v;
     theGraph->G[e].type = _ComputeArcType(theGraph, u, x, edgeType);

     e = gp_GetLastEdge(theGraph, x);
     context->G[e].pathConnector = w;
     theGraph->G[e].type = _ComputeArcType(theGraph, x, u, edgeType);

     /* Set the external face info */

     theGraph->extFace[u].link[0] = x;
     theGraph->extFace[x].link[1] = u;

     return OK;
}

/****************************************************************************
 _ReduceXYPathToEdge()
 ****************************************************************************/

int  _ReduceXYPathToEdge(graphP theGraph, K33SearchContext *context, int u, int x, int edgeType)
{
int  e, v, w, e_u0, e_u1, e_x0, e_x1;

     e = gp_GetFirstEdge(theGraph, u);
     e = gp_GetNextEdge(theGraph, e);
     v = theGraph->G[e].v;

     /* If the XY-path is a single edge, then no reduction is needed */

     if (v == x)
         return OK;

     /* Otherwise, remove the two edges that join the XY-path to the bicomp */

     if (context->G[e].pathConnector != NIL)
     {
         if (_RestoreReducedPath(theGraph, context, e) != OK)
             return NOTOK;
         e = gp_GetFirstEdge(theGraph, u);
         e = gp_GetNextEdge(theGraph, e);
         v = theGraph->G[e].v;
     }
     gp_DeleteEdge(theGraph, e, 0);

     e = gp_GetFirstEdge(theGraph, x);
     e = gp_GetNextEdge(theGraph, e);
     w = theGraph->G[e].v;
     if (context->G[e].pathConnector != NIL)
     {
         if (_RestoreReducedPath(theGraph, context, e) != OK)
             return NOTOK;
         e = gp_GetFirstEdge(theGraph, x);
         e = gp_GetNextEdge(theGraph, e);
         w = theGraph->G[e].v;
     }
     gp_DeleteEdge(theGraph, e, 0);

     /* Now add a single edge to represent the XY-path */

     e_u0 = gp_GetFirstEdge(theGraph, u);
     e_u1 = gp_GetLastEdge(theGraph, u);
     e_x0 = gp_GetFirstEdge(theGraph, x);
     e_x1 = gp_GetLastEdge(theGraph, x);
     gp_AddInternalEdge(theGraph, u, e_u0, e_u1, x, e_x0, e_x1);

     /* Now set up the path connectors so the original XY-path can be recovered if needed.
        Also, set the reduction edge's type to preserve the DFS tree structure */

     e = gp_GetFirstEdge(theGraph, u);
     e = gp_GetNextEdge(theGraph, e);
     context->G[e].pathConnector = v;
     theGraph->G[e].type = _ComputeArcType(theGraph, u, x, edgeType);

     e = gp_GetFirstEdge(theGraph, x);
     e = gp_GetNextEdge(theGraph, e);
     context->G[e].pathConnector = w;
     theGraph->G[e].type = _ComputeArcType(theGraph, x, u, edgeType);

     return OK;
}

/****************************************************************************
 _RestoreReducedPath()
 Given an edge record of an edge used to reduce a path, we want to restore
 the path in constant time.
 The path may contain more reduction edges internally, but we do not
 search for and process those since it would violate the constant time
 bound required of this function.
 return OK on success, NOTOK on failure
 ****************************************************************************/

int  _RestoreReducedPath(graphP theGraph, K33SearchContext *context, int J)
{
int  JTwin, u, v, w, x;
int  J0, J1, JTwin0, JTwin1;

     if (context->G[J].pathConnector == NIL)
         return OK;

     JTwin = gp_GetTwinArc(theGraph, J);

     u = theGraph->G[JTwin].v;
     v = context->G[J].pathConnector;
     w = context->G[JTwin].pathConnector;
     x = theGraph->G[J].v;

     /* Get the locations of the graph nodes between which the new
        graph nodes must be added in order to reconnect the path
        parallel to the edge. */

     J0 = gp_GetNextEdge(theGraph, J);
     J1 = gp_GetPrevEdge(theGraph, J);
     JTwin0 = gp_GetNextEdge(theGraph, JTwin);
     JTwin1 = gp_GetPrevEdge(theGraph, JTwin);

     /* Add the two edges to reconnect the reduced path. */

     if (gp_AddInternalEdge(theGraph, u, J, J0, v, v, gp_GetFirstEdge(theGraph, v)) != OK ||
         gp_AddInternalEdge(theGraph, x, JTwin, JTwin0, w, w, gp_GetFirstEdge(theGraph, w)) != OK)
         return NOTOK;

     /* Delete the edge represented by J and JTwin */

     gp_DeleteEdge(theGraph, J, 0);

     /* Set the types of the newly added edges */

     if (_SetEdgeType(theGraph, u, v) != OK ||
         _SetEdgeType(theGraph, w, x) != OK)
         return NOTOK;

     return OK;
}

/****************************************************************************
 _RestoreAndOrientReducedPaths()
 This function searches the embedding for any edges that are specially marked
 as being representative of a path that was previously reduced to a
 single edge by _ReduceBicomp().  The edge is replaced by the path.
 Note that the new path may contain more edges that are representative of a
 path, and these will be iteratively expanded.

 If the edge records of an edge being expanded are the link[0] or link[1]
 edge records of the edge's vertex endpoints, then the edge may be along
 the external face.  If so, then the vertices along the path being restored
 must be given a consistent orientation with the endpoints.  It is expected
 that the embedding will have been oriented prior to this operation.
 ****************************************************************************/

int  _RestoreAndOrientReducedPaths(graphP theGraph, K33SearchContext *context)
{
int  e, J, JTwin, u, v, w, x, visited;
int  J0, JTwin0, J1, JTwin1, PathIsOnExtFace;

     for (e = 0; e < theGraph->M + sp_GetCurrentSize(theGraph->edgeHoles);)
     {
         J = theGraph->edgeOffset + 2*e;
         if (context->G[J].pathConnector != NIL)
         {
             visited = theGraph->G[J].visited;

             JTwin = gp_GetTwinArc(theGraph, J);
             u = theGraph->G[JTwin].v;
             v = context->G[J].pathConnector;
             w = context->G[JTwin].pathConnector;
             x = theGraph->G[J].v;

             /* Now we need the predecessor and successor edge records
                of J and JTwin.  The edge (u, v) will be inserted so
                that the record in u's adjacency list that indicates v
                will be between J0 and J1.  Likewise, the edge record
                (x -> w) will be placed between JTwin0 and JTwin1. */

             J0 = gp_GetNextEdge(theGraph, J);
             J1 = gp_GetPrevEdge(theGraph, J);
             JTwin0 = gp_GetNextEdge(theGraph, JTwin);
             JTwin1 = gp_GetPrevEdge(theGraph, JTwin);

             /* We determine whether the reduction edge is on the external face,
                in which case we will need to ensure (below) that the vertices
                are consistently oriented.  This will accommodate future invocations
                of MarkPathAlongBicompExtFace().
                Note: If J0, J1, JTwin0 or JTwin1 is not an edge, then it is
                      because we've walked off the end of the edge record list,
                      which happens when J and JTwin are either the first or
                      last edge of the containing vertex.  In turn, the first
                      and last edges of a vertex are the ones that hold it onto
                      the external face. */

             if ((!gp_IsEdge(theGraph, J0) && !gp_IsEdge(theGraph, JTwin1)) ||
                 (!gp_IsEdge(theGraph, J1) && !gp_IsEdge(theGraph, JTwin0)))
                  PathIsOnExtFace = 1;
             else PathIsOnExtFace = 0;

             /* Now we add the two edges to reconnect the reduced path represented
                by the edge [J, JTwin].  The edge record in u is added between J and J1,
                but when edge [J, JTwin] is deleted, then the new edge record will be
                between J0 and J1.  Likewise, JTwin and the new edge record added to x. */

             if (gp_AddInternalEdge(theGraph, u, J, J1, v, v, gp_GetFirstEdge(theGraph, v)) != OK ||
                 gp_AddInternalEdge(theGraph, x, JTwin, JTwin1, w, w, gp_GetFirstEdge(theGraph, w)) != OK)
                 return NOTOK;

             /* We delete the edge represented by J and JTwin. We delete the edge after
                adding the path because if the deletion were done first, then one of
                the incident vertices might become degree 1, which would make it harder
                to add the edges that restore the reduced paths; in particular, it would
                be more difficult to determine which external face link should receive the
                new edge record being added.*/

             gp_DeleteEdge(theGraph, J, 0);

             /* Set the types of the newly added edges */

             if (_SetEdgeType(theGraph, u, v) != OK ||
                 _SetEdgeType(theGraph, w, x) != OK)
                 return NOTOK;

             /* If the path is along the external face, then we need it to act like an oriented
                path because we subsequently call functions like MarkPathAlongBicompExtFace() */

             if (PathIsOnExtFace)
                 if (_OrientPath(theGraph, u, v, w, x) != OK)
                     return OK;

             /* The internal XY path was already marked as part of the decision logic
                that made us decide we could find a K3,3 and hence that we should
                reverse all of the reductions.  Subsequent code counts on the fact
                that the X-Y path is already marked, so if we replace a marked edge
                with a path, then we need to mark the path. Similarly, for an unmarked
                edge, the replacement path should be unmarked. */

             _SetVisitedOnPath(theGraph, u, v, w, x, visited);
         }
         else e++;
     }

     return OK;
}

/****************************************************************************
 _SetEdgeType()
 When we are restoring an edge, we must restore its type.  We can deduce
 what the type was based on other information in the graph.
 ****************************************************************************/

int  _SetEdgeType(graphP theGraph, int u, int v)
{
int  e, eTwin, u_orig, v_orig, N;

     // If u or v is a virtual vertex (a root copy), then get the non-virtual counterpart.

     N = theGraph->N;
     u_orig = u < N ? u : (theGraph->V[u - N].DFSParent);
     v_orig = v < N ? v : (theGraph->V[v - N].DFSParent);

     // Get the edge for which we will set the type

     e = gp_GetNeighborEdgeRecord(theGraph, u, v);
     eTwin = gp_GetTwinArc(theGraph, e);

     // If u_orig is the parent of v_orig, or vice versa, then the edge is a tree edge

     if (theGraph->V[v_orig].DFSParent == u_orig ||
         theGraph->V[u_orig].DFSParent == v_orig)
     {
         if (u_orig > v_orig)
         {
             theGraph->G[e].type = EDGE_DFSPARENT;
             theGraph->G[eTwin].type = EDGE_DFSCHILD;
         }
         else
         {
             theGraph->G[eTwin].type = EDGE_DFSPARENT;
             theGraph->G[e].type = EDGE_DFSCHILD;
         }
     }

     // Otherwise it is a back edge

     else
     {
         if (u_orig > v_orig)
         {
             theGraph->G[e].type = EDGE_BACK;
             theGraph->G[eTwin].type = EDGE_FORWARD;
         }
         else
         {
             theGraph->G[eTwin].type = EDGE_BACK;
             theGraph->G[e].type = EDGE_FORWARD;
         }
     }

     return OK;
}

/****************************************************************************
 _OrientPath()
 ****************************************************************************/

int  _OrientPath(graphP theGraph, int u, int v, int w, int x)
{
int  e_u = gp_GetNeighborEdgeRecord(theGraph, u, v);
int  e_v, e_ulink, e_vlink;

    do {
        // Get the external face link in vertex u that indicates the
        // edge e_u which connects to the next vertex v in the path
    	// As a sanity check, we determine whether e_u is an
    	// external face edge, because there would be an internal
    	// implementation error if not
    	if (gp_GetFirstEdge(theGraph, u) == e_u)
    		e_ulink = 0;
    	else if (gp_GetLastEdge(theGraph, u) == e_u)
    		e_ulink = 1;
    	else return NOTOK;

        v = theGraph->G[e_u].v;

        // Now get the external face link in vertex v that indicates the
        // edge e_v which connects back to the prior vertex u.
        e_v = gp_GetTwinArc(theGraph, e_u);

    	if (gp_GetFirstEdge(theGraph, v) == e_v)
    		e_vlink = 0;
    	else if (gp_GetLastEdge(theGraph, v) == e_v)
    		e_vlink = 1;
    	else return NOTOK;

        // The vertices u and v are inversely oriented if they
        // use the same link to indicate the edge [e_u, e_v].
        if (e_vlink == e_ulink)
        {
            _InvertVertex(theGraph, v);
            e_vlink = 1^e_vlink;
        }

        theGraph->extFace[u].link[e_ulink] = v;
        theGraph->extFace[v].link[e_vlink] = u;

        u = v;
        e_u = theGraph->G[v].link[1^e_vlink];
    } while (u != x);

    return OK;
}

/****************************************************************************
 _SetVisitedOnPath()
 ****************************************************************************/

void _SetVisitedOnPath(graphP theGraph, int u, int v, int w, int x, int visited)
{
int  e = gp_GetNeighborEdgeRecord(theGraph, u, v);

     theGraph->G[v].visited = visited;

     do {
         v = theGraph->G[e].v;
         theGraph->G[v].visited = visited;
         theGraph->G[e].visited = visited;
         e = gp_GetTwinArc(theGraph, e);
         theGraph->G[e].visited = visited;

         e = gp_GetNextEdge(theGraph, e);
         if (gp_IsVertex(theGraph, e))
             e = gp_GetNextEdge(theGraph, e);
     } while (v != x);
}

/****************************************************************************
 _MarkStraddlingBridgePath()
 ****************************************************************************/

int  _MarkStraddlingBridgePath(graphP theGraph, int u_min, int u_max, int u_d, int d)
{
isolatorContextP IC = &theGraph->IC;
int p, J;

/* Find the point of intersection p between the path (v ... u_max)
       and the path (d ... u_max). */

     if (theGraph->functions.fpMarkDFSPath(theGraph, u_max, IC->r) != OK)
         return NOTOK;

     p = d;
     while (!theGraph->G[p].visited)
     {
         theGraph->G[p].visited = 1;

         J = gp_GetFirstEdge(theGraph, p);
         while (gp_IsEdge(theGraph, J))
         {
              if (theGraph->G[J].type == EDGE_DFSPARENT)
                  break;

              J = gp_GetNextEdge(theGraph, J);
         }

         theGraph->G[J].visited = 1;
         theGraph->G[gp_GetTwinArc(theGraph, J)].visited = 1;

         p = theGraph->G[J].v;

         /* If p is a root copy, mark it visited and skip to the parent copy */
         if (p >= theGraph->N)
         {
             theGraph->G[p].visited = 1;
             p = theGraph->V[p-theGraph->N].DFSParent;
         }
     }

/* Unmark the path (p ... u_max), which was marked to help find p.
    The path from v to u_{max} is not needed to form a K3,3 except
    for the portion of the path up to p that, with the straddling
    bridge path, comprises part of the connection to u_d. In the
    minor, the path between v and p is edge contracted. */

     while (p != u_max)
     {
         J = gp_GetFirstEdge(theGraph, p);
         while (gp_IsEdge(theGraph, J))
         {
              if (theGraph->G[J].type == EDGE_DFSPARENT)
                  break;

              J = gp_GetNextEdge(theGraph, J);
         }

         theGraph->G[J].visited = 0;
         theGraph->G[gp_GetTwinArc(theGraph, J)].visited = 0;

         p = theGraph->G[J].v;
         theGraph->G[p].visited = 0;

         /* If p is a root copy, clear its visited flag and skip to the
                parent copy */

         if (p >= theGraph->N)
         {
             p = theGraph->V[p-theGraph->N].DFSParent;
             theGraph->G[p].visited = 0;
         }
     }

/* The straddling bridge must join the path (u_max ... u_min).  If u_d is an
    ancestor of u_min, then mark the path that joins u_d to u_min. */

     if (u_d < u_min)
        if (theGraph->functions.fpMarkDFSPath(theGraph, u_d, u_min) != OK)
            return NOTOK;

     return OK;
}

/****************************************************************************
 _IsolateMinorE5()
 The paths (x, w), (y, w) and (v, u_{max}) are not needed.
 The x-y path and the internal w-z path are already marked.
 ****************************************************************************/

int  _IsolateMinorE5(graphP theGraph)
{
isolatorContextP IC = &theGraph->IC;

     if (_MarkPathAlongBicompExtFace(theGraph, IC->r, IC->x) != OK ||
         _MarkPathAlongBicompExtFace(theGraph, IC->y, IC->r) != OK ||
         theGraph->functions.fpMarkDFSPath(theGraph, MIN3(IC->ux,IC->uy,IC->uz),
                                                     MAX3(IC->ux,IC->uy,IC->uz)) != OK ||
         _MarkDFSPathsToDescendants(theGraph) != OK ||
         _JoinBicomps(theGraph) != OK ||
         _AddAndMarkUnembeddedEdges(theGraph) != OK)
         return NOTOK;

     return OK;
}

/****************************************************************************
 _IsolateMinorE6()
 The paths (x, y), (v, w) and (v, u_{max}) are not needed.
 The path through the straddling bridge that connects from an ancestor of
 u_{max} to v is required, but it may connnect to an ancestor p of v.
 In such a case, the path (v, p) is required, while (p, u_{max}) is not.
 ****************************************************************************/

int  _IsolateMinorE6(graphP theGraph, K33SearchContext *context)
{
isolatorContextP IC = &theGraph->IC;
int u_min, u_max, d, u_d;

/* Clear the previously marked x-y path */

     _FillVisitedFlagsInBicomp(theGraph, IC->r, 0);

/* Clear dw to stop the marking of path (v, w) */

     IC->dw = NIL;

/* Mark (v, ..., x, ..., w, ..., y, ... v) */

     if (_MarkPathAlongBicompExtFace(theGraph, IC->r, IC->r) != OK)
         return NOTOK;

/* Mark the path through the straddling bridge (except for the final
     edge (u_d, d) which is added last by convention). */

     u_min = MIN3(IC->ux,IC->uy,IC->uz);
     u_max = MAX3(IC->ux,IC->uy,IC->uz);
     d = _TestForStraddlingBridge(theGraph, context, u_max);
     u_d = theGraph->V[d].leastAncestor;

     if (_MarkStraddlingBridgePath(theGraph, u_min, u_max, u_d, d) != OK)
         return NOTOK;

/* Make the final markings and edge additions */

     if (theGraph->functions.fpMarkDFSPath(theGraph, u_min, u_max) != OK ||
         _MarkDFSPathsToDescendants(theGraph) != OK ||
         _JoinBicomps(theGraph) != OK ||
         _AddAndMarkUnembeddedEdges(theGraph) != OK ||
         _AddAndMarkEdge(theGraph, u_d, d) != OK)
         return NOTOK;

     return OK;
}

/****************************************************************************
 _IsolateMinorE7()
 ****************************************************************************/

int  _IsolateMinorE7(graphP theGraph, K33SearchContext *context)
{
isolatorContextP IC = &theGraph->IC;
int u_min, u_max, d, u_d;

/* Mark the appropriate two portions of the external face depending on
    symmetry condition */

     if (IC->uy < IC->ux)
     {
         if (_MarkPathAlongBicompExtFace(theGraph, IC->r, IC->x) != OK ||
             _MarkPathAlongBicompExtFace(theGraph, IC->w, IC->y) != OK)
             return NOTOK;
     }
     else
     {
         if (_MarkPathAlongBicompExtFace(theGraph, IC->x, IC->w) != OK ||
             _MarkPathAlongBicompExtFace(theGraph, IC->y, IC->r) != OK)
             return NOTOK;
     }

/* Mark the path through the straddling bridge (except for the final
     edge (u_d, d) which is added last by convention). */

     u_min = MIN3(IC->ux,IC->uy,IC->uz);
     u_max = MAX3(IC->ux,IC->uy,IC->uz);
     d = _TestForStraddlingBridge(theGraph, context, u_max);
     u_d = theGraph->V[d].leastAncestor;

     if (_MarkStraddlingBridgePath(theGraph, u_min, u_max, u_d, d) != OK)
         return NOTOK;

/* Make the final markings and edge additions */

     if (theGraph->functions.fpMarkDFSPath(theGraph, u_min, u_max) != OK ||
         _MarkDFSPathsToDescendants(theGraph) != OK ||
         _JoinBicomps(theGraph) != OK ||
         _AddAndMarkUnembeddedEdges(theGraph) != OK ||
         _AddAndMarkEdge(theGraph, u_d, d) != OK)
         return NOTOK;

     return OK;
}
