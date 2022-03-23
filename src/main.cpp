#include <stdlib.h> /* 亂數相關函數 */
#include <time.h>   /* 時間相關函數 */

#include <EularCircuit.hpp>
#include <chrono>
#include <fstream>

double find_match_violence(
  std::vector<std::vector<double>> & weight_map, std::vector<int> & not_match_yet,
  std::vector<std::pair<int, int>> & best_match)
{
  /*
  std::cout << "not match yet: ";
  for (int i = 0; i < not_match_yet.size(); i++) {
    std::cout << not_match_yet[i] << ", ";
  }
  std::cout << std::endl;
    */
  if (not_match_yet.size() == 2) {
    int a = not_match_yet.back();
    not_match_yet.pop_back();
    int b = not_match_yet.back();
    best_match.push_back(std::pair<int, int>(a, b));
    return weight_map[a][b];
  } else {
    double smallest = EC_MAX_DOUBLE_;
    int smallest_i = -1;
    std::vector<std::pair<int, int>> smallest_match;
    int a = not_match_yet.back();
    not_match_yet.pop_back();
    for (int i = 0; i < not_match_yet.size(); i++) {
      int b = not_match_yet.at(i);
      std::vector<int> remain = not_match_yet;
      std::vector<std::pair<int, int>> tmp_match;
      remain.erase(remain.begin() + i);
      double cur = weight_map[a][b] + find_match_violence(weight_map, remain, tmp_match);
      if (cur < smallest) {
        smallest = cur;
        smallest_i = i;
        smallest_match = tmp_match;
      }
    }
    best_match.push_back(std::pair<int, int>(a, not_match_yet[smallest_i]));
    best_match.insert(best_match.end(), smallest_match.begin(), smallest_match.end());
    /*
    std::cout << "best match: ";
    for (int i = 0; i < best_match.size(); i++) {
      std::cout << "(" << best_match[i].first << ", " << best_match[i].second << ") ";
    }
    std::cout << std::endl;
    */
    return smallest;
  }
}

int test_BM()
{
  srand(time(NULL));
  // Test the binary match class
  // Input test data
  int half = 7;
  int total = 2 * half;
  EC_binary_match bm(total);
  std::vector<std::vector<double>> weight(total, std::vector<double>(total, 0));
  std::vector<int> not_match(total);
  std::vector<std::pair<int, int>> match;

  // std::cout << "------- Weight -------" << std::endl;
  for (int i = 0; i < 2 * half; i++) {
    not_match[i] = i;
    for (int j = i + 1; j < 2 * half; j++) {
      double x = abs(100 * rand() / (RAND_MAX + 1.0));
      bm.insert_weight(i, j, x);
      weight[i][j] = x;
      weight[j][i] = x;
      // std::cout << x << " | ";
    }
    // std::cout << std::endl;
  }
  /*
  bm.insert_weight(0, 1, 2);
  bm.insert_weight(0, 2, 1);
  bm.insert_weight(0, 3, 4);
  bm.insert_weight(0, 4, 7);
  bm.insert_weight(0, 5, 10);
  bm.insert_weight(1, 2, 3);
  bm.insert_weight(1, 3, 6);
  bm.insert_weight(1, 4, 9);
  bm.insert_weight(1, 5, 6);
  bm.insert_weight(2, 3, 5);
  bm.insert_weight(2, 4, 12);
  bm.insert_weight(2, 5, 3);
  bm.insert_weight(3, 4, 11);
  bm.insert_weight(3, 5, 4);
  bm.insert_weight(4, 5, 2);
  */
  // calculate bm
  std::string key;
  bm.key_from_int_vector(key, not_match);
  std::cout << "key: " << key << std::endl;
  // violant
  std::cout << "------- VIOLENT -------" << std::endl;
  auto before = std::chrono::duration_cast<std::chrono::microseconds>(
                  std::chrono::system_clock::now().time_since_epoch())
                  .count();
  double spv = find_match_violence(weight, not_match, match);
  auto after = std::chrono::duration_cast<std::chrono::microseconds>(
                 std::chrono::system_clock::now().time_since_epoch())
                 .count();
  unsigned long us = after - before;
  std::cout << "shortest sum: " << spv << ", time (us): " << us << std::endl;
  for (int i = 0; i < half; i++) {
    std::pair<int, int> res = match.at(i);
    std::cout << "res " << i << " : " << res.first << ", " << res.second << std::endl;
  }
  // not violant
  std::cout << "------- NOT VIOLENT -------" << std::endl;
  before = std::chrono::duration_cast<std::chrono::microseconds>(
             std::chrono::system_clock::now().time_since_epoch())
             .count();
  double sp = bm.search_pairs(key);
  after = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now().time_since_epoch())
            .count();
  us = after - before;
  std::cout << "shortest sum: " << sp << ", time (us): " << us << std::endl;
  for (int i = 0; i < half; i++) {
    std::pair<int, int> res = bm.get_pairs(key, i);
    std::cout << "res " << i << " : " << res.first << ", " << res.second << std::endl;
  }
  return 0;
}

int test_EC()
{
  // define test set
  EC_graph eg(-1, -1, 6);
  // vertex
  eg.add_vert(0, 0, 0);
  eg.add_vert(1, 0, 1);
  eg.add_vert(2, 1, 1);
  eg.add_vert(3, 2, 1);
  eg.add_vert(4, 2, 0);
  eg.add_vert(5, 1, 0);
  // edge
  eg.add_edge(0, 1);
  eg.add_edge(1, 2);
  eg.add_edge(2, 3);
  eg.add_edge(3, 4);
  eg.add_edge(4, 5);
  eg.add_edge(5, 0);
  eg.add_edge(2, 5);
  /*
  // calculate odd vertices
  std::vector<v2d *> odd;
  eg.find_odd();
  eg.get_odd(odd);
  std::cout << "------- Find Odd -------" << std::endl;
  for (auto v : odd) {
    std::cout << v->index << ", ";
  }
  std::cout << std::endl;
  std::cout << "------- Update Path -------" << std::endl;
  double dist = eg.update_path(odd[0], odd[1]);
  std::cout << "shortest dist btw " << odd[0]->index << " and " << odd[1]->index << " is " << dist
            << std::endl;
            */
  /*
  std::cout << "------- Eularize -------" << std::endl;
  eg.eularize();
  std::string res;
  eg.print_width(res);
  std::cout << res << std::endl;
  */
  std::vector<v2d *> ec;
  eg.find_eular_circuit();
  eg.get_eular_circuit(ec);
  std::cout << "------- Eular Circuit -------" << std::endl;
  for (auto v : ec) {
    std::cout << v->index << ", ";
  }
  std::cout << std::endl;
  return 0;
}

int test_EC_testset2()
{
  std::cout << "------- Build Graph -------" << std::endl;
  // define test set
  EC_graph eg(-1, -1, 10);
  // vertex
  eg.add_vert(0, 36.1792, 76.1738);
  eg.add_vert(1, 93.8648, 97.2658);
  eg.add_vert(2, 2.22244, 70.8518);
  eg.add_vert(3, 58.2067, 26.8291);
  eg.add_vert(4, 28.8282, 4.09496);
  eg.add_vert(5, 2.5475, 25.3029);
  eg.add_vert(6, 22.3328, 94.5873);
  eg.add_vert(7, 25.2114, 72.4375);
  eg.add_vert(8, 12.183, 12.6488);
  eg.add_vert(9, 94.9887, 50.2166);
  // edge
  eg.add_edge(0, 3);
  eg.add_edge(0, 5);
  eg.add_edge(0, 8);
  eg.add_edge(1, 2);
  eg.add_edge(1, 5);
  eg.add_edge(1, 6);
  eg.add_edge(1, 7);
  eg.add_edge(1, 9);
  eg.add_edge(2, 5);
  eg.add_edge(2, 6);
  eg.add_edge(2, 7);
  eg.add_edge(2, 8);
  eg.add_edge(3, 4);
  eg.add_edge(3, 6);
  eg.add_edge(3, 7);
  eg.add_edge(3, 9);
  eg.add_edge(4, 7);
  eg.add_edge(4, 9);
  eg.add_edge(5, 9);
  eg.add_edge(6, 7);
  // print
  std::string res;
  eg.print_width(res);
  std::cout << res << std::endl;
  /*
  // calculate odd vertices
  std::vector<v2d *> odd;
  eg.find_odd();
  eg.get_odd(odd);
  std::cout << "------- Find Odd -------" << std::endl;
  for (auto v : odd) {
    std::cout << v->index << ", ";
  }
  std::cout << std::endl;
  std::cout << "------- Update Path -------" << std::endl;
  std::vector<std::vector<int>> visited(10, std::vector<int>(10, 0));
  double dist = eg.update_path(odd[4], odd[5], visited);
  std::cout << "shortest dist btw " << odd[0]->index << " and " << odd[1]->index << " is " << dist
            << std::endl;
  std::vector<v2d *> test;
  eg.get_path(odd[4], odd[5], test);
  std::cout << std::to_string(odd[4]->index);
  for (v2d * iv : test) {
    std::cout << " -> " << std::to_string(iv->index);
  }
  std::cout << std::endl;
  */
  /*
  std::cout << "------- Eularize -------" << std::endl;
  res.clear();
  eg.eularize();
  // std::string res;
  eg.print_width(res);
  std::cout << res << std::endl;
  */
  /*
  std::vector<v2d *> ec;
  eg.find_eular_circuit();
  eg.get_eular_circuit(ec);
  std::cout << "------- Eular Circuit -------" << std::endl;
  for (auto v : ec) {
    std::cout << v->index << ", ";
  }
  std::cout << std::endl;
  res.clear();
  eg.print_width(res);
  std::cout << res << std::endl;
  */

  // Now test the code
  std::cout << "------- Test Graph with Eular Circuit -------" << std::endl;
  std::cout << "------- Print the vertex here -------" << std::endl;
  int npt = 10;
  std::vector<v2d *> vv;
  std::vector<std::pair<int, int>> ve;
  eg.get_vert(vv);
  eg.get_edge(ve);
  for (int i = 0; i < npt; i++) {
    std::cout << "index: " << i << ", x: " << vv[i]->x << ", y: " << vv[i]->y << std::endl;
  }
  std::cout << "------- Print the edge here -------" << std::endl;
  for (int i = 0; i < ve.size(); i++) {
    std::cout << "index pair: " << ve[i].first << ", " << ve[i].second << std::endl;
  }

  // get the route

  std::vector<v2d *> ep;
  eg.find_eular_circuit();
  eg.get_eular_circuit(ep);
  std::cout << "------- Eular Circuit -------" << std::endl;
  for (auto v : ep) {
    std::cout << v->index << std::endl;
  }

  /*
  std::cout << std::endl;
  std::string res;
  res.clear();
  eg.print_width(res);
  std::cout << res << std::endl;
  */
  return 0;
}

int test_EC_rand()
{
  srand(time(NULL));
  int npt = 100, neg = 200;
  double rng = 100;
  // create a eular circuit graph
  std::cout << "------- Create Graph with Eular Circuit -------" << std::endl;
  std::vector<std::vector<int>> gph(npt, std::vector<int>(npt, 0));
  int curpt = abs((npt + 1) * (rand() / (RAND_MAX + 1.0)));
  int sttpt = curpt;
  std::cout << "go to " << curpt << " -> ";
  for (int i = 0; i < neg; i++) {
    bool ok = true;
    int nxtpt = -1;
    while (ok) {
      nxtpt = abs(npt * (rand() / (RAND_MAX + 1.0))) - 1;
      if ((nxtpt != curpt) && (gph[curpt][nxtpt] == 0)) {
        // The second last point cannot be the second point
        if ((i == neg - 1)) {
          if ((gph[nxtpt][sttpt] == 0)) {
            ok = false;
          }
        } else {
          ok = false;
        }
      }
    }
    std::cout << nxtpt << " -> ";
    gph[curpt][nxtpt] = 1;
    gph[nxtpt][curpt] = 1;
    curpt = nxtpt;
  }
  // make it back to the original point
  std::cout << sttpt << std::endl;
  gph[curpt][sttpt] = 1;
  gph[sttpt][curpt] = 1;
  // No test the code
  std::cout << "------- Test Graph with Eular Circuit -------" << std::endl;
  std::cout << "------- Print the vertex here -------" << std::endl;
  EC_graph ec(0, 0, npt);
  for (int i = 0; i < npt; i++) {
    double tx, ty;
    tx = rng * (rand() / (RAND_MAX + 1.0));
    ty = rng * (rand() / (RAND_MAX + 1.0));
    ec.add_vert(i, tx, ty);
    std::cout << "index: " << i << ", x: " << tx << ", y: " << ty << std::endl;
  }
  std::cout << "------- Print the edge here -------" << std::endl;
  for (int i = 0; i < npt; i++) {
    for (int j = i + 1; j < npt; j++) {
      if (gph[i][j] == 1) {
        ec.add_edge(i, j);
        std::cout << "index pair: " << i << ", " << j << std::endl;
      }
    }
  }
  // get the route
  std::vector<v2d *> ep;
  ec.find_eular_circuit();
  ec.get_eular_circuit(ep);
  std::cout << "------- Eular Circuit -------" << std::endl;
  for (auto v : ep) {
    std::cout << v->index << std::endl;
  }
  /*
  std::cout << std::endl;
  std::string res;
  res.clear();
  ec.print_width(res);
  std::cout << res << std::endl;
  */
  return 0;
}

int test_EC_rand_eularize()
{
  srand(time(NULL));
  int npt = 10, neg = 21;
  double rng = 100;
  // create a eular circuit graph
  std::cout << "------- Create Graph with Eular Circuit -------" << std::endl;
  std::vector<std::vector<int>> gph(npt, std::vector<int>(npt, 0));
  int rne = neg;  // remaining number of edges
  std::vector<int> flag(npt, 0);
  for (int i = 0; i < npt; i++) {
    if (flag[i] == 0) {
      bool ok = true;
      while (ok) {
        int j = i + 1 + abs((npt - i - 1.0) * (rand() / (RAND_MAX + 1.0)));
        std::cout << "j: " << j << std::endl;
        // std::cout << "(" << i << "," << j << ")" << std::endl;
        // if ((j < npt) && (flag[j] == 0)) {
        if (j < npt) {
          gph[i][j] = 1;
          gph[j][i] = 1;
          rne--;
          flag[i] = 1;
          flag[j] = 1;
          ok = false;
        }
      }
    }
    // choose a number between i+1 and npt
    if (rne < 0) {
      std::cout << "edge number must > vertex number" << std::endl;
      return 0;
    }
  }
  // randomly fill the remaining edges
  int ok = true;
  while (ok) {
    int i = abs((npt) * (rand() / (RAND_MAX + 1.0)));
    int j = abs((npt) * (rand() / (RAND_MAX + 1.0)));
    if ((i != j) && (gph[i][j] == 0)) {
      gph[i][j] = 1;
      gph[j][i] = 1;
      rne--;
      if (rne <= 0) {
        break;
      }
    }
  }
  for (int i = 0; i < npt; i++) {
    for (int j = 0; j < npt; j++) {
      std::cout << " | " << gph[i][j];
    }
    std::cout << " | " << std::endl;
  }
  // No test the code
  std::cout << "------- Test Graph with Eular Circuit -------" << std::endl;
  std::cout << "------- Print the vertex here -------" << std::endl;
  EC_graph ec(0, 0, npt);
  for (int i = 0; i < npt; i++) {
    double tx, ty;
    tx = rng * (rand() / (RAND_MAX + 1.0));
    ty = rng * (rand() / (RAND_MAX + 1.0));
    ec.add_vert(i, tx, ty);
    std::cout << "index: " << i << ", x: " << tx << ", y: " << ty << std::endl;
  }
  std::cout << "------- Print the edge here -------" << std::endl;
  for (int i = 0; i < npt; i++) {
    for (int j = i + 1; j < npt; j++) {
      if (gph[i][j] == 1) {
        ec.add_edge(i, j);
        std::cout << "index pair: " << i << ", " << j << std::endl;
      }
    }
  }
  // get the route
  std::vector<v2d *> ep;
  ec.find_eular_circuit();
  ec.get_eular_circuit(ep);
  std::cout << "------- Eular Circuit -------" << std::endl;
  for (auto v : ep) {
    std::cout << v->index << std::endl;
  }
  /*
  std::cout << std::endl;
  std::string res;
  res.clear();
  ec.print_width(res);
  std::cout << res << std::endl;
  */
  return 0;
}

void read_from_file()
{
  std::fstream file("EC.txt");
  if (!file) {
    std::cerr << "no file EC.txt found" << std::endl;
  }
  std::string buf;
  EC_graph * peg;
  std::vector<int> index;
  int state = -1;  // 0: read current position; 1: read vertex; 2: read edge; 3: read path
  while (getline(file, buf)) {
    if (0 == buf.compare("------- Current Position -------")) {
      state = 0;
    } else if (0 == buf.compare("------- Print the vertex here -------")) {
      state = 1;
    } else if (0 == buf.compare("------- Print the edge here -------")) {
      state = 2;
    } else if (0 == buf.compare("------- Eular Circuit -------")) {
      state = 3;
    } else {
      // std::cout << "at state " << state << std::endl;
      int num;
      int index;
      int ia, ib;
      int ip;
      double x, y;
      std::string gph;
      switch (state) {
        case 0:
          sscanf(buf.c_str(), "x: %lf, y: %lf, total num: %d", &x, &y, &num);
          peg = new EC_graph(x, y, num);
          peg->print_width(gph);
          // std::cout << gph << std::endl;
          break;
          // std::cout << "x: " << x << ", y: " << y << ", total num: " << num << std::endl;
        case 1:
          sscanf(buf.c_str(), "index: %d, x: %lf, y: %lf", &index, &x, &y);
          peg->add_vert(index, x, y);
          break;
          // std::cout << "x: " << x << ", y: " << y << ", index: " << index << std::endl;
        case 2:
          sscanf(buf.c_str(), "index pair: %d, %d", &ia, &ib);
          // std::cout << "ia: " << ia << ", ib: " << ib << std::endl;
          peg->add_edge(ia, ib);
          break;
        case 3:
          sscanf(buf.c_str(), "%d", &ip);
          break;
      }
    }
  }
  /*
  std::string gph;
  peg->print_width(gph);
  std::cout << gph << std::endl;
  */
  // get path
  peg->find_eular_circuit();
  peg->print_file("ectest.txt");
}

int main()
{
  // test_EC_rand_eularize();
  read_from_file();
  return 0;
}
