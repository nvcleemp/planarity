# Introduction #

The new algorithm added to Planarity-Related Graph Algorithms package, version 2.1.0, is a method for isolating a subgraph homeomorphic to K<sub>4</sub> in a given graph.  This page reports the results of running that new algorithm.

As with the other algorithm implementations in the package, the new implementation does extensive error checking.  The results were checked on all graphs with 11 or fewer vertices in both debug and release modes on a Windows-based and an Intel-Linux computer.  In release mode, the results for 11 vertex graphs took just over 33000 seconds on a 2.4GHz CPU.

To round out testing of version 2.1.0, the results for the core planarity algorithm and the search for subgraphs homeomorphic to K<sub>3,3</sub> were retested, and the same results were obtained as reported in TestResultsForVersion200.

# Details #

This section reports the results of executing the search for K<sub>4</sub> algorithm on all 11 vertex graphs.  The outerplanarity algorithm was used as a basis, with some additional code refactored due to commonalities with the algorithms for finding a subgraph homeomorphic to K<sub>3,3</sub>.

Any time an outerplanarity obstruction is found, it is either the desired subgraph homeomorphic to K<sub>4</sub> or it is a subgraph homeomorphic to K<sub>2,3</sub>.  In the latter case, several tests are executed to determine whether a desire subgraph homeomorphic to K<sub>4</sub> is entangled with the subgraph homeomorphic to K<sub>2,3</sub>.  If not, then one or two components of the graph each separable by a 2-cut are reduced to one or two single edges.  This allows the core outerplanarity algorithm to proceed.

The _# OKs_ column below reports the number of graphs in which a subgraph homeomorphic to K<sub>4</sub> was not found.  The _# NoEmbeds_ column tells the number of graphs in which a subgraph homeomorphic to K<sub>4</sub> was found.  The integrity checker would, in this case, then ensure that the result is a subgraph of the original graph and that it has the structure of a K<sub>4</sub> homeomorph. These results were obtained using the following command-line:

  * ./planarity.exe -gens -4 11

It is interesting to note that the _# OKs_ column number tapers to zero after 19 edges, in accordance with the result of Asano [1](1.md) that any graph with more than 2n-3 edges is guaranteed to contain a subgraph homeomorphic to K<sub>4</sub>.

```
# Edges  # graphs    # OKs       # NoEmbeds
-------  ----------  ----------  ----------
      0           1           1           0
      1           1           1           0
      2           2           2           0
      3           5           5           0
      4          11          11           0
      5          26          26           0
      6          67          66           1
      7         172         169           3
      8         467         452          15
      9        1305        1236          69
     10        3664        3343         321
     11       10250        8756        1494
     12       28259       21227        7032
     13       75415       43998       31417
     14      192788       72306      120482
     15      467807       88368      379439
     16     1069890       75977      993913
     17     2295898       43085     2252813
     18     4609179       14397     4594782
     19     8640134        2171     8637963
     20    15108047           0    15108047
     21    24630887           0    24630887
     22    37433760           0    37433760
     23    53037356           0    53037356
     24    70065437           0    70065437
     25    86318670           0    86318670
     26    99187806           0    99187806
     27   106321628           0   106321628
     28   106321628           0   106321628
     29    99187806           0    99187806
     30    86318670           0    86318670
     31    70065437           0    70065437
     32    53037356           0    53037356
     33    37433760           0    37433760
     34    24630887           0    24630887
     35    15108047           0    15108047
     36     8640134           0     8640134
     37     4609179           0     4609179
     38     2295898           0     2295898
     39     1069890           0     1069890
     40      467807           0      467807
     41      192788           0      192788
     42       75415           0       75415
     43       28259           0       28259
     44       10250           0       10250
     45        3664           0        3664
     46        1305           0        1305
     47         467           0         467
     48         172           0         172
     49          67           0          67
     50          26           0          26
     51          11           0          11
     52           5           0           5
     53           2           0           2
     54           1           0           1
     55           1           0           1
Totals   1018997864      375597  1018622267
```

`[1]` Takao ASANO. An Approach to the Subgraph Homeomorphism Problem. _Theoretical Computer Science_ Volume 38, pp. 249-267, 1985.