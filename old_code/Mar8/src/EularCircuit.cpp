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

EC_vertex::EC_vertex(double x, double y, std::string & label)
{
  p_.x = x;
  p_.y = y;
  label_ = label;
}

void EC_vertex::get_pose(p2D & p) { p = p_; }

void EC_vertex::get_label(std::string & label) { label = label_; }

int EC_vertex::check_adj_unwalked(EC_vertex * v)
{
  for (int i = 0; i < adj_unwalked_.size(); i++) {
    double tx = adj_unwalked_.at(i)->p_.x;
    double ty = adj_unwalked_.at(i)->p_.y;
    if ((tx == v->p_.x) && (ty == v->p_.y)) {
      return EC_EXIST_;
    }
  }
  return EC_NOT_EXIST_;
}

void EC_vertex::adj_insert(
  EC_vertex * v, int bidirect = EC_BIDIRECT_, int no_repeat_adj = EC_NOREPEAT_ADJ_)
{
  // If no repeat adjacent vertex is allowed
  if (no_repeat_adj == EC_NOREPEAT_ADJ_) {
    if (check_adj_unwalked(v) == EC_NOT_EXIST_) {
      adj_unwalked_.push_back(v);
      if (bidirect == EC_BIDIRECT_) {
        v->adj_insert(this);
      }
    }
  } else {
    adj_unwalked_.push_back(v);
    if (bidirect == EC_BIDIRECT_) {
      v->adj_insert(this);
    }
  }
}

int EC_vertex::even_edge()
{
  int num = adj_walked_.size() + adj_unwalked_.size();
  if ((num % 2) == 0) {
    return EC_EVEN_;
  } else {
    return EC_ODD_;
  }
}

EC_graph::EC_graph(double curX, double curY)
{
  curX_ = curX;
  curY_ = curY;
  start_vertex_.first = EC_MAX_DOUBLE_;
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
  double distv = dist(v, curX_, curY_);
  // update closest vertex to curX_ and Y_
  if (distv < start_vertex_.first) {
    start_vertex_.first = distv;
    start_vertex_.second = v;
  }
  // maintain max_index_
  if (v->index > max_index_) {
    max_index_ = v->index;
    // resize the graph matrixes and fill them
    e2d init;
    init.width = 0;
    init.dist = EC_MAX_DOUBLE_;
    mat_dist_.resize(max_index_, std::vector<e2d>(max_index_, init));
  }
}

void EC_graph::add_edge(
  v2d * a, v2d * b, int bidirect = EC_BIDIRECT_, int no_repeat_adj = EC_NOREPEAT_ADJ_)
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
  mat_dist_[a->index][b->index].dist = dist(a, b);
  mat_dist_[a->index][b->index].path.push_back(b);
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
}

void EC_graph::add_edge(
  int ia, int ib, int bidirect = EC_BIDIRECT_, int no_repeat_adj = EC_NOREPEAT_ADJ_)
{
  add_edge(map_ver_[ia], map_ver_[ib], bidirect, no_repeat_adj);
}

void EC_graph::find_odd(int bidirect = EC_BIDIRECT_)
{
  if (bidirect == EC_BIDIRECT_) {
    for (int i = 0; i < max_index_; i++) {
      int i_edge = 0;
      for (int j = 0; j < max_index_; j++) {
        i_edge += mat_dist_[i][j].width;
      }
      if ((i_edge % 2) == 0) {
        odd_vertex_.push_back(map_ver_[i]);
      }
    }
  } else {
    // Not implemented yet
    return;
  }
}

double EC_graph::update_path(v2d * v1, v2d * v2, int bidirect = EC_BIDIRECT_)
{
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
        return mat_dist_[v1->index][v2->index].dist;
      }
    }
  }
  /* Check boundary conditions: if v1 == v2, then we return 0
   */
  if (v1 == v2) {
    return 0;
  }
  /* else if the mat_dist_ value is not the
   * initial value, then we will return it immediately cause it is
   * already shortest path.
   */
  else if (mat_dist_[v1->index][v2->index].dist != EC_MAX_DOUBLE_) {
    return mat_dist_[v1->index][v2->index].dist;
  }
  /* If the B.C. is not met, we will recursively find shortest path.
   */
  double shortest = EC_MAX_DOUBLE_;
  int mid_pt = -1;
  for (int i = 0; i < max_index_; i++) {
    double tdist = update_path(v1, map_ver_[i]) + update_path(map_ver_[i], v2);
    if (tdist < shortest) {
      shortest = tdist;
      mid_pt = i;
    }
  }
  mat_dist_[v1->index][v2->index].dist = shortest;
  mat_dist_[v1->index][v2->index].path.insert(
    mat_dist_[v1->index][v2->index].path.end(), mat_dist_[v1->index][mid_pt].path.begin(),
    mat_dist_[v1->index][mid_pt].path.end());
  mat_dist_[v1->index][v2->index].path.insert(
    mat_dist_[v1->index][v2->index].path.end(), mat_dist_[mid_pt][v2->index].path.begin(),
    mat_dist_[mid_pt][v2->index].path.end());
  return shortest;
}

void EC_graph::eularize(int bidirect = EC_BIDIRECT_)
{
  // We first find all vertice with odd edges
  find_odd(bidirect);
  //
}
