#include <EularCircuit.hpp>

double dmax(double a, double b)
{
  if (a > b) {
    return a;
  } else {
    return b;
  }
}

double dmin(double a, double b)
{
  if (a < b) {
    return a;
  } else {
    return b;
  }
}

bool v2d_comp(v2d * a, v2d * b) { return b->index > a->index; }

void EC_binary_match::insert_weight(int ai, int bi, double w)
{
  // Assume symmetric weight matrix, we make the second index always larger
  // then the first index because our index in the key is arranged from small
  // to big
  int big, small;
  big = dmax(ai, bi);
  small = dmin(ai, bi);
  // record the weight
  weight_[small][big] = w;
  std::string key;
  key = std::to_string(small) + _c + std::to_string(big) + _c;
  map_bm_[key].first = w;
  map_bm_[key].second.push_back(std::pair<int, int>(small, big));
}

void EC_binary_match::key_from_int(std::string & key, int i)
{
  for (int j = 0; j < i; j++) {
    key += (std::to_string(j) + _c);
  }
}

void EC_binary_match::key_from_int_vector(std::string & key, std::vector<int> & vec)
{
  for (int iv : vec) {
    key += (std::to_string(iv) + _c);
  }
}

int EC_binary_match::key_split(std::string & key, std::vector<int> & index)
{
  if (key.size() < 1) {
    return EC_NOT_EXIST_;
  } else {
    std::string sub_key;
    for (char c : key) {
      if (c != _c.c_str()[0]) {
        sub_key += c;
      } else {
        index.push_back(std::stoi(sub_key));
        sub_key.clear();
      }
    }
    return EC_EXIST_;
  }
}

std::string EC_binary_match::key_remove(std::string key, int i_sub_key)
{
  std::string sub_key;
  sub_key = std::to_string(i_sub_key) + _c;
  key.erase(key.find(sub_key), sub_key.size());
  return key;
}

double EC_binary_match::search_pairs(std::string key, int key_len)
{
  /*
  std::cout << "key: " << key << std::endl;
  */
  auto bm = map_bm_.find(key);
  // check if this sub BM found before
  if (bm != map_bm_.end()) {
    // std::cout << "found: " << bm->second.first << std::endl;
    return bm->second.first;
  }
  // count key_len if not defined
  if (key_len == -1) {
    key_len = std::count(key.begin(), key.end(), '_');
  }
  // compare matches {(0,1), remain}, {(0,2), remain}... and find
  // the match with the smallest weight sum.
  // 1. split the key and handle the sub key
  std::vector<int> index;
  key_split(key, index);
  std::string sk_no_0 = key_remove(key, index[0]);
  // 2. we need some variable to record the smallest value
  std::string smallest_sk;
  double smallest_sum = EC_MAX_DOUBLE_;
  int smallest_match = -1;
  // 3. do the comparison
  for (int i = 1; i < key_len; i++) {
    std::string sk_no_i = key_remove(sk_no_0, index[i]);
    double i_sum = weight_[index[0]][index[i]] + search_pairs(sk_no_i);
    if (i_sum < smallest_sum) {
      smallest_sum = i_sum;
      smallest_match = i;
      smallest_sk = sk_no_i;
    }
  }
  // update the sum of weight of the current key
  map_bm_[key].first = smallest_sum;
  // update the best match of the current key
  map_bm_[key].second.push_back(std::pair<int, int>(index[0], index[smallest_match]));
  map_bm_[key].second.insert(
    map_bm_[key].second.end(), map_bm_[smallest_sk].second.begin(),
    map_bm_[smallest_sk].second.end());
  // return
  return smallest_sum;
}

EC_graph::EC_graph(double curX, double curY, int ver_num)
{
  curX_ = curX;
  curY_ = curY;
  start_vertex_.first = EC_MAX_DOUBLE_;
  vertex_num_ = ver_num;
  mat_dist_.resize(
    vertex_num_, std::vector<e2d>(
                   vertex_num_, {.weight = EC_MAX_DOUBLE_,
                                 .dist = EC_MAX_DOUBLE_,
                                 .width = 0,
                                 .path = std::vector<v2d *>()}));
}

double EC_graph::dist(v2d * v, double x, double y)
{
  return sqrt(pow((v->x - x), 2) + pow((v->y - y), 2));
}

double EC_graph::dist(v2d * v1, v2d * v2)
{
  return sqrt(pow((v1->x - v2->x), 2) + pow((v1->y - v2->y), 2));
}

void EC_graph::add_vert(v2d * v)
{
  map_ver_[v->index] = v;
  // update closest vertex to curX_ and Y_
  double distv = dist(v, curX_, curY_);
  if (distv < start_vertex_.first) {
    start_vertex_.first = distv;
    start_vertex_.second = v;
  }
}

void EC_graph::add_edge(v2d * a, v2d * b, int bidirect, int no_repeat_adj)
{
  /* If repeated adjacent vertex is not allowed, and
   * there is already edges between a and b, then we
   * do nothing here.
   */
  if (no_repeat_adj == EC_NOREPEAT_ADJ_) {
    if (mat_dist_[a->index][b->index].width > 0) {
      return;
    }
  }
  mat_dist_[a->index][b->index].width += 1;
  mat_dist_[a->index][b->index].weight = dist(a, b);
  mat_dist_[a->index][b->index].path.push_back(b);
  // maintain the shortest adj.
  if (a->shortest_adj_index == EC_NOT_INDEX_) {
    a->shortest_adj_dist = mat_dist_[a->index][b->index].weight;
    a->shortest_adj_index = b->index;
  } else {
    if (mat_dist_[a->index][b->index].weight < a->shortest_adj_dist) {
      a->shortest_adj_dist = mat_dist_[a->index][b->index].weight;
      a->shortest_adj_index = b->index;
    }
  }
  lastest_edge_ = false;
  if (bidirect == EC_BIDIRECT_) {
    add_edge(b, a, !(bidirect), no_repeat_adj);
  }
  // a->adj_insert(b, bidirect, no_repeat_adj);
}

void EC_graph::add_vert(int index, double x, double y)
{
  v2d * pv = new v2d;
  pv->index = index;
  pv->x = x;
  pv->y = y;
  add_vert(pv);
}

void EC_graph::add_edge(int ia, int ib, int bidirect, int no_repeat_adj)
{
  add_edge(map_ver_[ia], map_ver_[ib], bidirect, no_repeat_adj);
}

void EC_graph::find_odd(int bidirect)
{
  if (bidirect == EC_BIDIRECT_) {
    for (int i = 0; i < vertex_num_; i++) {
      int i_edge = 0;
      for (int j = 0; j < vertex_num_; j++) {
        i_edge += mat_dist_[i][j].width;
      }
      if ((i_edge % 2) == 1) {
        odd_vertex_.push_back(map_ver_[i]);
      }
    }

    std::sort(odd_vertex_.begin(), odd_vertex_.end(), v2d_comp);
  } else {
    // Not implemented yet
    return;
  }
}

double EC_graph::update_path(
  v2d * v1, v2d * v2, std::vector<std::vector<int>> & visited, int bidirect)
{
  // If updated edges before, update the shortest adj vertex distances to the edge's "dist"
  if (!lastest_edge_) {
    for (auto pv : map_ver_) {
      mat_dist_[pv.first][pv.second->shortest_adj_index].dist = pv.second->shortest_adj_dist;
      if (bidirect == EC_BIDIRECT_) {
        mat_dist_[pv.second->shortest_adj_index][pv.first].dist = pv.second->shortest_adj_dist;
      }
    }
    lastest_edge_ = true;
  }
  /* If bidirectional, we may check whether
   * - mat_dist_[v1][v2] does not exist, AND
   * - mat_dist_[v2][v1] exists. 
   */
  if (bidirect == EC_BIDIRECT_) {
    if (mat_dist_[v1->index][v2->index].dist == EC_MAX_DOUBLE_) {
      if (mat_dist_[v2->index][v1->index].dist != EC_MAX_DOUBLE_) {
        // copy the distance
        mat_dist_[v1->index][v2->index].dist = mat_dist_[v2->index][v1->index].dist;
        // copy the path in reverse
        mat_dist_[v1->index][v2->index].path.insert(
          mat_dist_[v1->index][v2->index].path.end(),
          mat_dist_[v2->index][v1->index].path.rbegin() + 1,
          mat_dist_[v2->index][v1->index].path.rend());
        visited[v1->index][v2->index] == 0;
        return mat_dist_[v1->index][v2->index].dist;
      }
    }
  }
  /* Check boundary conditions: 
   * If v1 == v2, then we return 0
   */
  if (v1 == v2) {
    visited[v1->index][v2->index] == 0;
    return 0;
  }
  /* We will return it immediately if it is
   * already the shortest path.
   */
  else if (mat_dist_[v1->index][v2->index].dist != EC_MAX_DOUBLE_) {
    visited[v1->index][v2->index] == 0;
    return mat_dist_[v1->index][v2->index].dist;
  }
  /* If the pair is visited in the upper stack of update_path, then this will never be
   * the shortest pair
   */
  else if (visited[v1->index][v2->index] == 1) {
    visited[v1->index][v2->index] ==
      2;  // This means this pair is visited before and currently no any solution
    return EC_MAX_DOUBLE_;
  } else if (visited[v1->index][v2->index] == 2) {
    return EC_MAX_DOUBLE_;
  }
  /* If the B.C. is not met, we will recursively find shortest path.
   */
  double shortest = mat_dist_[v1->index][v2->index].weight;
  int mid_pt = -1;
  visited[v1->index][v2->index] = 1;
  for (int i = 0; i < vertex_num_; i++) {
    if ((i != v1->index) && (i != v2->index)) {
      double tdist = update_path(v1, map_ver_[i], visited) + update_path(map_ver_[i], v2, visited);
      if (tdist < shortest) {
        shortest = tdist;
        mid_pt = i;
      }
    }
  }
  mat_dist_[v1->index][v2->index].dist = shortest;
  if (mid_pt != -1) {
    mat_dist_[v1->index][v2->index].path.insert(
      mat_dist_[v1->index][v2->index].path.begin(), mat_dist_[mid_pt][v2->index].path.begin(),
      mat_dist_[mid_pt][v2->index].path.end());
    mat_dist_[v1->index][v2->index].path.insert(
      mat_dist_[v1->index][v2->index].path.begin(), mat_dist_[v1->index][mid_pt].path.begin(),
      mat_dist_[v1->index][mid_pt].path.end());
    visited[v1->index][v2->index] == 0;
  }
  return shortest;
}

void EC_graph::connect_odd_vertice()
{
  // Do nothing if no odd vertices
  if (odd_vertex_.size() == 0) {
    return;
  }
  // create binary matcher
  EC_binary_match my_bm(odd_vertex_.size());
  // produce the key for the binary match
  std::string main_key;
  my_bm.key_from_int(main_key, odd_vertex_.size());
  // produce the weight matrix for my_bm
  std::vector<std::vector<int>> visited(vertex_num_, std::vector<int>(vertex_num_, 0));
  for (int i = 0; i < odd_vertex_.size(); i++) {
    for (int j = i + 1; j < odd_vertex_.size(); j++) {
      update_path(odd_vertex_[i], odd_vertex_[j], visited);
      my_bm.insert_weight(i, j, mat_dist_[odd_vertex_[i]->index][odd_vertex_[j]->index].dist);
    }
  }
  // search for best matches
  my_bm.search_pairs(main_key);
  // For each match, we add bidirectional shortest path to original graph
  for (int i = 0; i < (odd_vertex_.size() / 2); i++) {
    std::pair<int, int> op = my_bm.get_pairs(main_key, i);
    v2d * last_pov = odd_vertex_[op.first];
    for (v2d * pov : mat_dist_[odd_vertex_[op.first]->index][odd_vertex_[op.second]->index].path) {
      add_edge(last_pov, pov, EC_BIDIRECT_, !(EC_NOREPEAT_ADJ_));
      last_pov = pov;
    }
  }
}

void EC_graph::eularize(int bidirect)
{
  // A. We first find all vertice with odd edges, this function
  //    also sorts the odd_vertex_
  find_odd(bidirect);
  // B. Then we pair those odd edges so that the total sum of the
  //    distance of each pair can be the smallest.
  //    After that, we add pathes of those paired odd_vertices to original graphs.
  if (bidirect == EC_BIDIRECT_) {
    connect_odd_vertice();
  } else {
    // Not implemented yet
    return;
  }
}

void EC_graph::print_width(std::string & res)
{
  for (int i = 0; i < vertex_num_; i++) {
    for (int j = 0; j < vertex_num_; j++) {
      double dis = mat_dist_[i][j].width;
      res += std::to_string(dis) + " | ";
    }
    res += "\r\n";
  }
}

void EC_graph::find_eular_circuit(int bidirect)
{
  // A. We first "Eularize" the graph so that eular circuit exists.
  eularize(bidirect);
  // B. started with closest vertex, we can then searching for eular circuits.
  v2d * cv = start_vertex_.second;  // current vertex
  v2d * sv = cv;                    // start vertex of a sub_circuit
  eular_path_.push_back(cv);        // Add the first vertex to the Eular path
  int path_iter_flag =
    0;  // the new vertex to the eular circuit should be added at eular_path_.begin()+path_iter_flag
  bool done = false;
  while (!done) {
    bool no_adj = true;
    // Find any adjacent vertex
    // std::cout << "unfinished size: " << unfinished_.size() << std::endl;

    // std::string res;
    // print_width(res);
    // std::cout << res;

    for (int i = 0; i < vertex_num_; i++) {
      // std::cout << "width from cv:" << cv->index << "to i: " << i << " : "
      //          << mat_dist_[cv->index][i].width << std::endl;
      if (mat_dist_[cv->index][i].width > 0) {
        // Adj. vertex found, go there.
        no_adj = false;
        path_iter_flag += 1;
        // When go to another vertex, delete the edge between this and that.
        // If bidirectional, we will have to delete the edge symmetrically.
        mat_dist_[cv->index][i].width--;
        if (bidirect == EC_BIDIRECT_) {
          mat_dist_[i][cv->index].width--;
        }
        // If original vertex still has other adjacent vertices, added to the unfinished stack
        int edge_num = 0;
        for (int j = 0; j < vertex_num_; j++) {
          edge_num += mat_dist_[cv->index][j].width;
        }
        if (edge_num != 0) {
          unfinished_.push_back(std::pair<int, v2d *>(path_iter_flag - 1, cv));
        }
        // update the iter_flag, current vertex to the next one, and add the next one to the Eular circuit
        cv = map_ver_[i];
        eular_path_.insert(eular_path_.begin() + path_iter_flag, cv);
        // std::cout << "path iter flag: " << path_iter_flag << ", path size: " << eular_path_.size()
        //          << std::endl;
        break;
      }
    }
    // If no adjacent vertex for cv, then it must be the completion of the current sub circuit
    // (where cv == sv) or we met the dead end (should not happen).
    // If unfinished_ is empty, then we have done the eular circuit.
    if (no_adj) {
      if (cv != sv) {
        // Error
        // std::cout << "Error" << std::endl;
        return;
      } else if (unfinished_.size() == 0) {
        done = true;
        // std::cout << "Done" << std::endl;
      } else {
        // When the sub circuit done, we go to the unfinished vertices
        std::pair<int, v2d *> last_unf = unfinished_.back();
        path_iter_flag = last_unf.first;
        // std::cout << "Go to iter: " << path_iter_flag << std::endl;
        cv = last_unf.second;
        sv = cv;
        unfinished_.pop_back();
      }
    }
  }
}
