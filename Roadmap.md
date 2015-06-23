This page presents the roadmap for future development of this project.

## Version 2.x ##

  * Planar Embedding and Kuratowski Subgraph Isolation (**DONE** in v2.0)
  * Outerplanar Embedding and Obstruction Isolation (**DONE** in v2.0)
  * Extensible Planarity-Related Algorithm Framework (**DONE** in v2.0)
  * Planar Graph Drawing by Visibility Representation (**DONE** in v2.0)
  * K23 subgraph homeomorphism (**DONE** in v2.0)
  * K33 subgraph homeomorphism (**DONE** in v2.0)

  * K4 subgraph homeomorphism (**DONE** in v2.1)
  * Vertex coloring by minimal degree selection, including linear time planar graph 5-coloring with the Matula-Shiloach-Tarjan-Frederickson optimization (**DONE** in v2.2)

## Version 2.x.y ##

Minor adjustments and tweaks that may be desired from time to time. This may include optimizations and service routines such as:

  * Edge contract and uncontract (**DONE** in v2.2.1)
  * Identifying two vertices, i.e. merging two vertices (**DONE** in v2.2.1)
  * Consolidated DFS and Lowpoint Preprocessing Optimization (**DONE** in v2.2.1)

## Version 3.0 ##

The internal data structures will be revisited to separate the "graph node" into separate definitions for half edges (arcs), vertices, and virtual vertices. Although graph nodes were originally created to enable vertices and virtual vertices to be linked into their adjacency lists, it has been clear for many years that this is not necessary, nor is it preferable from the software engineering standpoint of understandability of the library.

Well, at least this is how it started out.  As the data structure modifications were being made, many aspects of the core processing model as well as coding patterns were also addressed, including:

  * Recoding of data structure access using function-like macros to facilitate future porting and hide data structure access details from the main code
  * Higher precision core planarity processing model based on sorted DFS child lists, enabling Walkdown to detect non-planarity via immediate comparison with sorted forward arc list.
  * New future pertinence/external activity detection mechanism
  * Improved model for allowing extension to handle embedding obstructions
  * Removed all extension mechanisms from K4 search except the path connector for reducing obstructions
  * Changed all vertex and edge iteration code to consistent, more readable naming conventions
  * Lots of refinements, such as improved twin arc operation, removal of inversion flag from external face optimization, and improved use of the K33 mergeBlocker optimization

These improvements to the data structures and APIs are now **DONE**.

## Version 3.x ##

Versions of this library may be created with other languages, such as C++ and Java.

Other algorithms that may be formulated in the future could be supported as extensions, such as:

  * Maximal planar subgraph
  * K5 subgraph homeomorphism
  * Projective planarity

A fuller range of algorithm implementations may also be developed, such as

  * Planar graph dual
  * Triconnected component isolator
  * BFS
  * More supported I/O formats
  * Directed graph methods, e.g. topological sort, 2SAT
  * Weighted graph methods, esp. Dijkstra's shortest path with a Fibonacci heap.
  * An O(log n) improvement to the Karger-Stein randomized min-cut method