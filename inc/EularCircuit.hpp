#ifndef EC__H
#define EC__H
#include <math.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

#define EC_EXIST_ 0
#define EC_NOT_EXIST_ 0
#define EC_BIDIRECT_ 1
#define EC_NOREPEAT_ADJ_ 1
#define EC_NOT_INDEX_ -1
#define EC_MAX_DOUBLE_ 1e300
#define EC_MIN_DOUBLE_ 1e-300

/* In this code, we can use EC_graph to build graph and find Eular Circuit that traverse every edges.
 * EC_binary_match match the elements 2 by 2 with minimal sum of cost, given costs between arbitrary 2 elements.
 * However, EC_binary_match won't be used by external program, tho, if we only want the Eular Circuit.
 */

typedef struct EC_vertex_2D
{
  double x, y;
  int index;
  double shortest_adj_dist = EC_MAX_DOUBLE_;
  int shortest_adj_index = EC_NOT_INDEX_;
} v2d;

typedef struct EC_edge_2D
{
  double weight;
  double dist;  // sum of shortest route
  int width;
  std::vector<v2d *> path;
} e2d;

/* Assumption on matches we handle here:
 * - the direction of a certain match (A, B) or (B, A)
 *   won't affects the weight.
 */
class EC_binary_match
{
public:
  /* The number of total elements we want to match. 
   * Must be an even number.
   */
  EC_binary_match(int num)
  {
    weight_.resize(num, std::vector<double>(num, 0));
    _c = std::string("_");
  }

  /* Insert the cost between 2 different elements. 
   */
  void insert_weight(int ai, int bi, double w);

  /* Get the map key.  We store known combination of pairs in a map.  
   */
  void key_from_int(std::string & key, int i);

  /* Get the map key.  We store known combination of pairs in a map.  
   */
  void key_from_int_vector(std::string & key, std::vector<int> & vec);

  /* Make the key (string) an array of integers.  
   */
  int key_split(std::string & key, std::vector<int> & index);

  /* Remove a sub_key (i_sub_key, key can be composed by several sub_key)
   */
  std::string key_remove(std::string key, int i_sub_key);

  /* We match the pairs here.
   */
  double search_pairs(std::string key, int key_len = -1);

  /* Get the matched result.
   */
  std::pair<int, int> get_pairs(std::string key, int ith_pair)
  {
    return map_bm_[key].second[ith_pair];
  }

private:
  // We can search sub-binary match using a string (key) that represent vertices in the sub-binary match
  std::map<std::string, std::pair<double, std::vector<std::pair<int, int>>>> map_bm_;

  // The 2D vector that stores the weight between vertices.
  std::vector<std::vector<double>> weight_;

  // some char to string stuff, we need a character so that
  // we can delete index in the string successfully
  std::string _c;
};

/* Assumption for graphes we handled here:
 * - Every vertex in the graph has unique position and label
 * - Every vertex has some way to go to another other vertex
 */
class EC_graph
{
public:
  /* param:
   * - curX   : robot start point X
   * - curY   : robot start point Y
   * - ver_num: number of total vertice
   * It is required to specify the number of the vertice in the graph at start and
   * can't be change afterward
   */
  EC_graph(double curX, double curY, int ver_num);

  /* distance of a vertex v and a point (x, y)
   */
  double dist(v2d * v, double x, double y);

  /* distance of 2 vertice v1 and v2
   */
  double dist(v2d * v1, v2d * v2);

  /* Add the vertex v into the graph.
   * The total number of addition can't exceed ver_num specified
   * in the constructor.
   */
  void add_vert(v2d * v);

  /* Add the edge of vertex a and b into the graph.
   * By default, the graph is non directed.  Curently this class can't
   * handle directed graph.
   */
  void add_edge(
    v2d * a, v2d * b, int bidirect = EC_BIDIRECT_, int no_repeat_adj = EC_NOREPEAT_ADJ_);

  /* Add the vertex named index at (x, y) into the graph.
   * The total number of addition can't exceed ver_num specified
   * in the constructor.
   * The index must be non-negative integral and the number must be smaller than ver_num specified
   * in the constructor.
   */
  void add_vert(int index, double x, double y);

  /* Add the edge of vertice with index ia and ib into the graph.
   * By default, the graph is non directed.  Curently this class can't
   * handle directed graph.
   */
  void add_edge(int ia, int ib, int bidirect = EC_BIDIRECT_, int no_repeat_adj = EC_NOREPEAT_ADJ_);

  /* 
   */
  void get_vert(std::vector<v2d *> & vv)
  {
    for (int i = 0; i < vertex_num_; i++) {
      vv.push_back(map_ver_[i]);
    }
  }

  /* 
   */
  void get_edge(std::vector<std::pair<int, int>> & ve)
  {
    for (int i = 0; i < vertex_num_; i++) {
      for (int j = 0; j < vertex_num_; j++) {
        e2d te = mat_dist_[i][j];
        if (te.weight != EC_MAX_DOUBLE_) {
          ve.push_back(std::pair<int, int>(i, j));
        }
      }
    }
  }

  /* Get the vertex pointer by its index.
   */
  v2d * get_p_vertex(int index) { return map_ver_[index]; }

  /* Find vertice with odd edges in current graph. 
   * It will sort the odd_vertex_ by its index, from small to big.
   * TODO: If it is not EC_BIDIRECT_, then we will find 
   * vertice with not equal number of in-edges and
   * out-edges.
   */
  void find_odd(int bidirect = EC_BIDIRECT_);

  /* Get the odd_vertex_ array that stored vertice with odd edges after
   * find_odd()
   */
  void get_odd(std::vector<v2d *> & odd) { odd = odd_vertex_; }

  /* Update shortest distance of v1 and v2 in mat_graph_.
   * We must initialize a 2D vector of integral "visited" first.
   * The size must be ver_num * ver_num specified in the constructor.
   */
  double update_path(
    v2d * v1, v2d * v2, std::vector<std::vector<int>> & visited, int bidirect = EC_BIDIRECT_);

  /*  */
  void get_path(v2d * v1, v2d * v2, std::vector<v2d *> & res);

  /* After find_odd(), this function can give a serie of odd-vertice pairs with minimal
   * sum of distances.
   * We assume the graph is not directed.
   */
  void connect_odd_vertice();

  /* Make the graph suitable for generating Eular Circuit.  That is, we make every vertex have even edges.
   */
  void eularize(int bidirect = EC_BIDIRECT_);

  /* Stored the Adjacency Map in the string "res".
   */
  void print_width(std::string & res);

  /* 
   */
  // void print_path();

  /* Calculate the Eular Circuit.
   */
  void find_eular_circuit(int bidirect = EC_BIDIRECT_);

  /* The vector of vertice is the sequence for visiting the vertex on Eular Circuit.
   */
  void get_eular_circuit(std::vector<v2d *> & ec) { ec = eular_path_; }

private:
  // If edge updated, it become false. When false, we must update edge's lowest dist before update_path().
  bool lastest_edge_ = true;

  // Current robot position.  we need this for the start point of the Eular Circuit.
  double curX_, curY_;

  // The total number of the vertice.
  int vertex_num_ = 0;

  // It means (start_vertex distance to curX_/Y_, vertex)
  std::pair<double, v2d *> start_vertex_;

  // We can access the vertex with its index
  std::map<int, v2d *> map_ver_;

  // The adjacency matrix here.
  std::vector<std::vector<e2d>> mat_dist_;

  // The odd_matrix_ found using find_odd()
  std::vector<v2d *> odd_vertex_;

  // We store unvisited Eular circuit branches here.
  std::vector<std::pair<int, v2d *>> unfinished_;

  // The eventual Eular Circuit.
  std::vector<v2d *> eular_path_;
};

#endif