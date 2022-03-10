# R_Tree
## Introduction
- This inpliments a 2D R Tree with insertion and KNN search algorithm.
- This is made of c++.
- It requires standard c/c++ library.
- Developed on Ubuntu 20.04
- If you are not familiar with R Tree just like I used to be, please go to Reference first. There are many useful links found on Internet!
## Usage
### Terms
- Items: Tree elements that represent data that are stored in the tree. They can be either edges with start point and end point or just bounding boxes.
- Nodes: Nodes are tree elements which are not items. However, I called both Nodes and Items "Nodes" in codes and comments. It could be confusing, though.
- Leafs: They are nodes which contain only items. Therefore they are leaf nodes but not items.
- Root: Literally root of current tree. It is a node whose parent is itself.
- All above stuffs are the same class "rtree" with different flags.
### Install
```bash=
git clone https://github.com/TieneSabor/R_Tree
# Now we can test the code
cd R_Tree
make all
./bin/R_Tree_Test
```
### Test Code
- Test Code did following things.
    <!-- - Originally insert 7 edges, where 6 of them are on the same line. -->
    - Insert edges from ./data/nodes01.txt.
    - Print the tree.
    <!-- - Search which node contains the point (1.5,1.5) -->
    - Search 3 nearest item in the graph to (4,2.5)
    <!-- - Delete an item -->
    <!-- - Print again -->
### APIs
- Please refer to "rtree.hpp" in ./inc for detailed function definition.
- Some notes:
    - The first node is special: It is not an item, and it is a root and a leaf, so we will use a function to initialize it.
    - Every action starts from certain node and change only the subtree where the certain node is the root. Thus if we want to modify the whole tree, we need to find the root first.
    - The root node may changes everytime we insert a new item. Therefore we must find the new root after every insertion.
#### To Build a new tree:
```cpp=
#include "rtree.hpp"
/* ... */
// Initialize the first node.
rtree first_node;
first_node.set_the_first_node();
// Do some insertions.
// We insert an edge first. The edge start from (1,1) and end in (2,2)
first_node.rt_root()->rt_insert_edge(1,1,2,2);
// Then an item with bounding box (xmin,ymin), (xmax,ymax): (2,2), (3,3)
first_node.rt_root()->rt_insert(2,2,3,3);
// We can print the tree now!
first_node.rt_root()->rt_print();
```
#### DO a KNN search:
```cpp=
/* Assume we already have a tree */
// We want to find top 3 nearest items relate to (1.5,1.5) in the R tree.
std::vector<rtree*> result2;
first_node.rt_root()->rt_k_nearest_items(1.5,1.5,3,&result2);
// The last item in the vector "result2" is the nearest item. And the first item will be the 3rd nearest one. 
```

## Note and Problems
- The deletion of the item is incomplete. That is, the tree won't rebalance or check underflow after deletion. This is because in my current application, I don't delete items that often.
    - Therefore any operation after deletion may cause fault because it may lead to nodes with no child in a tree.
- It is very welcome to suggest how to validate the correctness of this code. Maybe there is a open source test set for R tree somewhere. Or maybe there is a simple way to produce various dataset to test it.
- Actually all kind of suggestions are very welcome.

## Reference
### Concepts
- [Since R tree can be extensions of B tree](https://en.wikipedia.org/wiki/B-tree)
- [A clear tutorial on R Tree](http://www.mathcs.emory.edu/~cheung/Courses/554/Syllabus/3-index/R-tree.html)
- [Another clear tutorial, with KNN search](https://slidetodoc.com/spatial-indexing-for-nn-retrieval-rtree-rtrees-n/)
- [A difference between linear and quadratic split](https://stackoverflow.com/questions/17289393/difference-between-quadratic-split-and-linear-split)
- [Closest distance between a point and a square](https://gamedev.stackexchange.com/questions/171967/getting-closest-distance-between-a-squares-edge-and-a-point-2d)
- [A complete tutorial in Chinese](https://dsa.cs.tsinghua.edu.cn/~deng/cg/project/2009f/2009f-2-f.pdf)
### R Tree in Python
- [OSMNX module Nearest edge function](https://osmnx.readthedocs.io/en/stable/osmnx.html)
- [R Tree library in Python](https://rtree.readthedocs.io/en/latest/class.html)
### Discussion in KNN search
- KD Tree
- [StackOverflow Discussion of nearest edge in graph](https://stackoverflow.com/questions/19892564/find-nearest-edge-in-graph)
- [StackOverflow discussion of Using Quad Tree to solve it](https://stackoverflow.com/questions/29471249/finding-the-nearest-neighboring-edge-in-a-quadtree)
