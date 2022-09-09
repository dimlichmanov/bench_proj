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

#define INT_ELEMENTS_PER_EDGE 3.0
#define VECTOR_ENGINE_THRESHOLD_VALUE VECTOR_LENGTH*MAX_SX_AURORA_THREADS*128
#define VECTOR_CORE_THRESHOLD_VALUE 16*VECTOR_LENGTH

#include "graph_library.h"

// -----------------------------------------------------------------------------------------------


void Init(VectCSRGraph &graph, int scale)
{
    EdgesListGraph el_graph;
    int vertices_count = pow(2.0, scale);
    int edges_count = 32*vertices_count;
    //GraphGenerationAPI::random_uniform(el_graph, vertices_count, edges_count, DIRECTED_GRAPH);

    // Warning! Graph vertices is reordered and renumbered here. You can use special VGL API functions to renumber vertices.
    graph.import(el_graph);
}

struct AlgorithmStats
{
    double wall_time;
    double ops_done;
    double sustained_bw;
    double wall_perf;
};

AlgorithmStats Kernel(VectCSRGraph &graph, VerticesArray<int> &levels)
{
    int source_vertex = graph.select_random_vertex(ORIGINAL);

    performance_stats.reset_timers();
    BFS::nec_top_down(graph, levels, source_vertex);
    performance_stats.update_timer_stats();

    AlgorithmStats stats;
    stats.wall_perf = performance_stats.get_avg_perf(graph.get_edges_count()) / 1e6;
    stats.sustained_bw = performance_stats.get_sustained_bandwidth() / 1e9;
    stats.ops_done = performance_stats.get_edges_rate() / 1e6;
    stats.wall_time = performance_stats.get_inner_time();

    return stats;
}

#endif
