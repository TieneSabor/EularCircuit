#ifndef EC__H
#define EC__H
#include <math.h>

#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

#define EC_EVEN_ 0
#define EC_ODD_ 1
#define EC_EXIST_ 0
#define EC_NOT_EXIST_ 0
#define EC_SAME_POSE_ 1
#define EC_BIDIRECT_ 1
#define EC_NOREPEAT_ADJ_ 1
#define EC_MAX_DOUBLE_ 1e300
#define EC_MIN_DOUBLE_ 1e-300

typedef struct EC_point2D
{
  double x, y;
} p2D;

/* Assumption for graphes we handled here:
 * - Every vertex in the graph has unique position and label
 * - Every vertex has some way to go to another other vertex
 */

typedef struct EC_vertex_2D
{
  double x, y;
  int index;
} v2d;

typedef struct EC_edge_2D
{
  double dist;
  int width;
  std::vector<v2d *> path;
} e2d;

class EC_binary_match
{
public:
private:
};

class EC_graph
{
public:
  /*  */
  EC_graph(double curX, double curY);

  /*  */
  double dist(v2d * v, double x, double y);

  /*  */
  double dist(v2d * v1, v2d * v2);

  /*  */
  void add_vert(v2d * v);

  /*  */
  void add_edge(
    v2d * a, v2d * b, int bidirect = EC_BIDIRECT_, int no_repeat_adj = EC_NOREPEAT_ADJ_);

  /*  */
  void add_vert(int index, double x, double y);

  /*  */
  void add_edge(int ia, int ib, int bidirect = EC_BIDIRECT_, int no_repeat_adj = EC_NOREPEAT_ADJ_);

  /* Find vertice with odd edges in current graph.
   * If it is not EC_BIDIRECT_, then we will find 
   * vertice with not equal number of in-edges and
   * out-edges.
   */
  void find_odd(int bidirect = EC_BIDIRECT_);

  /* update shortest distance of v1 and v2 in mat_graph_
   */
  double update_path(v2d * v1, v2d * v2, int bidirect = EC_BIDIRECT_);

  /* 
   */
  void connect_odd_vertice();

  /* 
   */
  void eularize(int bidirect = EC_BIDIRECT_);

  /*  */
  void find_eular_circuit(int bidirect = EC_BIDIRECT_);

private:
  double curX_, curY_;
  int max_index_ = 0;
  std::pair<double, v2d *> start_vertex_;  // <start_vertex distance to curX_/Y_, vertex>
  std::map<int, v2d *> map_ver_;
  std::vector<std::vector<e2d>> mat_dist_;
  std::vector<v2d *> odd_vertex_;
  std::vector<v2d *> unfinished_;
  std::vector<v2d *> eular_path_;
};

#endif