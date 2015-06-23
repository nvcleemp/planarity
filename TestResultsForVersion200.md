Test Results for Planarity-Related Graph Algorithms Package, version 2.0.0

# Introduction #

This page provides the results obtained from executing the various algorithms of the Planarity-Related Graph Algorithms package, version 2.0.0.

The graph generator from the Brendan McKay's nauty package was incorporated for the purposes of testing the planarity-related graph algorithm implementations on all graphs of given orders (numbers of vertices) and sizes (numbers of edges).  This page reports results for each size of graph of order 11.  This includes graphs that have isolated vertices, are disconnected, and have cut vertices, not just graphs that are biconnected.

For each algorithm, every graph is provided, the algorithm is performed, and then an integrity check on the result is performed. Depending on the algorithm, this testing of all 11 vertex graphs takes between 28000 and 46000 seconds on a late 2001 model Windows-based computer with a 2GHz CPU and 1GB memory.  The same results have also been obtained on a similar vintage Intel-based computer running Linux.

The sections below provide the tables for the various algorithms tested, along with further details about each algorithm.  The numerics are intrinsically interesting and also demonstrate various well-known mathematical results.

# Planar Graph Embedding, Drawing and Obstruction Isolation #

The core planar graph embedding algorithm produces a combinatorial planar embedding for each planar graph and a subgraph homeomorphic to K{3,3} or K5 for each non-planar graph.  The integrity test for the planar embedding does an embedding face count, and the integrity test for the non-planar graphs ensures that the result is a subgraph of the original graph and that it does have precisely the structure of a K{3,3} or K5 homeomorph. The table in this section is generated with the following command line:

  * ./planarity.exe -gens -p 11

The table below is also generated as the result of the planar graph drawing algorithm.  The numerics are the same as for the core planarity algorithm, as one would expect, but the algorithm integrity checking is different.  Each non-planar graph is simply ignored. For each planar graph, as the core planarity algorithm runs, key information needed to generate a visibility representation is collected. The embedder post-processing function is defined to generate the visibility representatino, and the integrity check function is augmented to test that in fact none of the vertical or horizontal lines of the visibility representation intersect, except for edge lines meeting up with the lines representing their vertex endpoints.  These results are produced with the following command line:

  * ./planarity.exe -gens -d 11

The column labelled _# OKs_ can be interpreted as the number of planar graphs found.  Notice how this number tapers off to zero after 27 edges, due to the well-known Euler limit of 3n-6 edges for planar graphs (n=11 in these tests).

```
# Edges  # graphs    # OKs       # NoEmbeds
-------  ----------  ----------  ----------
      0           1           1           0
      1           1           1           0
      2           2           2           0
      3           5           5           0
      4          11          11           0
      5          26          26           0
      6          67          67           0
      7         172         172           0
      8         467         467           0
      9        1305        1304           1
     10        3664        3659           5
     11       10250       10222          28
     12       28259       28094         165
     13       75415       74407        1008
     14      192788      186664        6124
     15      467807      433303       34504
     16     1069890      903573      166317
     17     2295898     1642808      653090
     18     4609179     2535353     2073826
     19     8640134     3252691     5387443
     20    15108047     3407101    11700946
     21    24630887     2866079    21764808
     22    37433760     1897492    35536268
     23    53037356      964363    52072993
     24    70065437      361848    69703589
     25    86318670       94684    86223986
     26    99187806       15362    99172444
     27   106321628        1249   106320379
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
Totals   1018997864    18681008  1000316856
```

# Outerplanar Graph Embedding and Obstruction Isolation #

This section reports the results of executing the outerplanarity algorithms on all 11 vertex graphs.  Each outerplanar graph is checked to ensure it is a combinatorial planar embedding, but also that all vertices are on the outer face.  For each non-outerplanar graph, a subgraph homeomorphic to K{2,3} or K4 is obtained.  The integrity check ensures it is a subgraph of the original graph and that it has the structure of a K{2,3} or K4 homeomorph. These results were obtained using the following command-line:

  * ./planarity.exe -gens -o 11

```
# Edges  # graphs    # OKs       # NoEmbeds
-------  ----------  ----------  ----------
      0           1           1           0
      1           1           1           0
      2           2           2           0
      3           5           5           0
      4          11          11           0
      5          26          26           0
      6          67          65           2
      7         172         164           8
      8         467         429          38
      9        1305        1135         170
     10        3664        2916         748
     11       10250        7017        3233
     12       28259       14771       13488
     13       75415       25183       50232
     14      192788       32642      160146
     15      467807       30656      437151
     16     1069890       19952     1049938
     17     2295898        8450     2287448
     18     4609179        2090     4607089
     19     8640134         228     8639906
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
Totals   1018997864      145744  1018852120
```

# Searching for a Subgraph Homeomorphic to K{2,3} #

This section reports the results of executing the search for K{2,3} algorithm on all 11 vertex graphs.  The outerplanarity algorithms are used as a basis.  If the graph is found to be outerplanar, then it does not contain a subgraph homeomorphic to K{2,3}.  Any time an outerplanarity obstruction is found, it is either the desired subgraph homeomorphic to K{2,3} or it is a subgraph homeomorphic to K4.  In the latter case, several tests are executed to determine whether a desire subgraph homeomorphic to K{2,3} is entangled with the subgraph homeomorphic to K4.  If not, the subgraph homeomorphic to K4 is reduced to a non-obstruction, allowing the outerplanarity algorithm to proceed.  The _# OKs_ column below reports the number of graphs in which a subgraph homeomorphic to K{2,3} was not found.  The _# NoEmbeds_ column tells the number of graphs in which a subgraph homeomorphic to K{2,3} was found.  The integrity checker would, in this case, then ensure that the result is a subgraph of the original graph and that it has the structure of a K{2,3} homeomorph. These results were obtained using the following command-line:

  * ./planarity.exe -gens -2 11

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
      7         172         166           6
      8         467         435          32
      9        1305        1153         152
     10        3664        2969         695
     11       10250        7169        3081
     12       28259       15193       13066
     13       75415       26181       49234
     14      192788       34459      158329
     15      467807       33188      434619
     16     1069890       22391     1047499
     17     2295898        9967     2285931
     18     4609179        2629     4606550
     19     8640134         318     8639816
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
Totals   1018997864      156330  1018841534
```

# Searching for a Subgraph Homeomorphic to K{3,3} #

This section reports the results of executing the search for K{3,3} algorithm on all 11 vertex graphs.  The planarity algorithms are used as a basis.  If the graph is found to be planar, then it does not contain a subgraph homeomorphic to K{3,3}.  Any time an planarity obstruction is found, it is either the desired subgraph homeomorphic to K{3,3} or it is a subgraph homeomorphic to K5.  In the latter case, several tests are executed to determine whether a desire subgraph homeomorphic to K{3,3} is entangled with the subgraph homeomorphic to K5.  If not, the subgraph homeomorphic to K5 is reduced to a non-obstruction, allowing the planarity algorithm to proceed.  The _# OKs_ column below reports the number of graphs in which a subgraph homeomorphic to K{3,3} was not found.  The _# NoEmbeds_ column tells the number of graphs in which a subgraph homeomorphic to K{3,3} was found.  The integrity checker would, in this case, then ensure that the result is a subgraph of the original graph and that it has the structure of a K{3,3} homeomorph. These results were obtained using the following command-line:

  * ./planarity.exe -gens -3 11

It is interesting to note that the _# OKs_ column number tapers to zero after 28 edges, in accordance with the result of Asano that any graph with more than 3n-5 edges is guaranteed to contain a subgraph homeomorphic to K{3,3}.

```
  # Edges       # Graphs       no K3,3     with K3,3
------------  ------------  ------------  ------------
           0             1             1             0  
           1             1             1             0  
           2             2             2             0  
           3             5             5             0  
           4            11            11             0  
           5            26            26             0  
           6            67            67             0  
           7           172           172             0  
           8           467           467             0  
           9          1305          1304             1  
          10          3664          3660             4  
          11         10250         10225            25  
          12         28259         28108           151  
          13         75415         74471           944  
          14        192788        186965          5823  
          15        467807        434696         33111  
          16       1069890        909557        160333  
          17       2295898       1660599        635299  
          18       4609179       2574581       2034598  
          19       8640134       3319556       5320578  
          20      15108047       3499929      11608118  
          21      24630887       2973476      21657411  
          22      37433760       2000850      35432910  
          23      53037356       1045199      51992157  
          24      70065437        412001      69653436  
          25      86318670        118507      86200163  
          26      99187806         23427      99164379  
          27     106321628          2961     106318667  
          28     106321628           179     106321449  
          29      99187806             0      99187806  
          30      86318670             0      86318670  
          31      70065437             0      70065437  
          32      53037356             0      53037356  
          33      37433760             0      37433760  
          34      24630887             0      24630887  
          35      15108047             0      15108047  
          36       8640134             0       8640134  
          37       4609179             0       4609179  
          38       2295898             0       2295898  
          39       1069890             0       1069890  
          40        467807             0        467807  
          41        192788             0        192788  
          42         75415             0         75415  
          43         28259             0         28259  
          44         10250             0         10250  
          45          3664             0          3664  
          46          1305             0          1305  
          47           467             0           467  
          48           172             0           172  
          49            67             0            67  
          50            26             0            26  
          51            11             0            11  
          52             5             0             5  
          53             2             0             2  
          54             1             0             1  
          55             1             0             1  
     Totals     1018997864      19281003     999716861  
```