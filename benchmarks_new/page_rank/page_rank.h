#ifndef BFS_H
#define BFS_H

#include <stdexcept>
#include <cmath>
#include <limits>
#include <list>
#include <cstring>
#include <array>
#include <vector>


// --------------------------------------------- VGL ---------------------------------------------

#define __USE_MULTICORE__

//#define __PRINT_API_PERFORMANCE_STATS__
//#define __PRINT_SAMPLES_PERFORMANCE_STATS__

#define INT_ELEMENTS_PER_EDGE 5.0
#define VECTOR_ENGINE_THRESHOLD_VALUE 2147483646
#define VECTOR_CORE_THRESHOLD_VALUE 5*VECTOR_LENGTH

#include "graph_library.h"

// -----------------------------------------------------------------------------------------------


void Init(int core_type, VectCSRGraph &graph, int scale)
{
    EdgesListGraph el_graph;
    int vertices_count = pow(2.0, scale);
    int edges_count = 32*vertices_count;

    if(core_type == 0)
        GraphGenerationAPI::random_uniform(el_graph, vertices_count, edges_count, DIRECTED_GRAPH);
    else if( core_type == 1)
        GraphGenerationAPI::R_MAT(el_graph, vertices_count, edges_count, 57, 19, 19, 5, DIRECTED_GRAPH);

    // Warning! Graph vertices is reordered and renumbered here. You can use special VGL API functions to renumber vertices.
    graph.import(el_graph);
}

void Kernel(VectCSRGraph &graph, VerticesArray<float> &page_ranks)
{
    float convergence_factor = 1.0e-4;
    PageRank::nec_page_rank(graph, page_ranks, convergence_factor);
}

#endif
