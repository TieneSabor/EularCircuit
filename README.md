# Eular Circuit
## Introduction
- This is an edge traversing algorithm for a given graph.
- To traversing every edge, we need to add some extra widthes on current edges to the graph.  In this implementation, we make this addition minimum in their distance.
- It requires standard c/c++ library.
- Developed on Ubuntu 20.04
## Usage
### Vertex Structures
- v2d: has fellowing members:
    - index: the name of the vertex
    - x: x position
    - y: y position
- v2d and its pointer is the basic element for our graph operation. 
### Install
```bash=
git clone https://github.com/TieneSabor/EularCircuit
# Now we can test the code
cd EularCircuit
make all
./bin/Eular_Circuit_Test
# If you want to print the result for later visualization:
./bin/Eular_Circuit_Test > xxx.txt
# Visualize the graph and Eular circuit
# The python script requires matplotlib
python3 ./script/show_graph.py -p xxx.txt
```
### Test Code
- Test Code did following things.
    - Randomly create a graph with 10 vertice and 21 edges.
        - Change the number by changing npt and neg in the test_ec_rand_eularize()
    - Print every vertex and edge
    - Start with the vertice closest to the point (0, 0).
    - Give a sequence of vertice for traversing.
### APIs
- Please refer to "EularCircuit.hpp" in ./inc for detailed function definition.
#### To Build a new tree:
```cpp=
#include "EularCircuit.hpp"
/* ... */
// Initialize with starting point and vertex numbers (assuming (0,0) and 10).
EC_graph ec(0, 0, 10);
// Add all the vertice first
// Current the index must be a non-negative integer sequence, like 0, 1, 2...
ec.add_vert(index, x, y);
// Then add the edges that connect 2 vertice added before by assigning their indice
ec.add_edge(index_1, index_2)
// Now we can find the traversing sequences
ec.find_eular_circuit();
// Then we must prepare a vector of vertex pointer to store the result
std::vector<v2d *> ep;
ec.get_eular_circuit(ep);
// We can print the path now!
for (auto v : ep) {
    std::cout << v->index << std::endl;
}
```

## Note and Problems

## Reference
