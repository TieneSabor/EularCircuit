#include <stdlib.h> /* 亂數相關函數 */
#include <time.h>   /* 時間相關函數 */

#include <EularCircuit.hpp>
#include <chrono>

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

int test_EC_rand()
{
  srand(time(NULL));
  int npt = 20, neg = 40;
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
  EC_graph ec(0, 0, npt);
  for (int i = 0; i < npt; i++) {
    double tx, ty;
    tx = rng * (rand() / (RAND_MAX + 1.0));
    ty = rng * (rand() / (RAND_MAX + 1.0));
    ec.add_vert(i, tx, ty);
  }
  for (int i = 0; i < npt; i++) {
    for (int j = i + 1; j < npt; j++) {
      // std::cout << " | " << gph[i][j];
      if (gph[i][j] == 1) {
        ec.add_edge(i, j);
      }
    }
    // std::cout << " | " << std::endl;
  }
  std::string res;
  ec.print_width(res);
  std::cout << res << std::endl;
  // get the route
  std::vector<v2d *> ep;
  ec.find_eular_circuit();
  ec.get_eular_circuit(ep);
  std::cout << "------- Eular Circuit -------" << std::endl;
  for (auto v : ep) {
    std::cout << v->index << " -> ";
  }
  std::cout << std::endl;
  res.clear();
  ec.print_width(res);
  std::cout << res << std::endl;
  return 0;
}

int test_EC_rand_eularize()
{
  srand(time(NULL));
  int npt = 10, neg = 20;
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
        // std::cout << "(" << i << "," << j << ")" << std::endl;
        if ((j < npt) && (flag[j] == 0)) {
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
  EC_graph ec(0, 0, npt);
  for (int i = 0; i < npt; i++) {
    double tx, ty;
    tx = rng * (rand() / (RAND_MAX + 1.0));
    ty = rng * (rand() / (RAND_MAX + 1.0));
    ec.add_vert(i, tx, ty);
  }
  for (int i = 0; i < npt; i++) {
    for (int j = i + 1; j < npt; j++) {
      if (gph[i][j] == 1) {
        ec.add_edge(i, j);
      }
    }
  }
  // std::string res;
  // ec.print_width(res);
  // std::cout << res << std::endl;
  // get the route
  std::vector<v2d *> ep;
  ec.find_eular_circuit();
  ec.get_eular_circuit(ep);
  std::cout << "------- Eular Circuit -------" << std::endl;
  for (auto v : ep) {
    std::cout << v->index << " -> ";
  }
  std::cout << std::endl;
  std::string res;
  res.clear();
  ec.print_width(res);
  std::cout << res << std::endl;
  return 0;
}

int main()
{
  test_EC_rand_eularize();
  return 0;
}