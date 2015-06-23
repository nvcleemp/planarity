# Introduction #

The new extension algorithm added to the Planarity-Related Graph Algorithms package, version 2.2.0, is a method for coloring graphs by minimum degree selection. The method includes a special neighborhood contraction technique for degree 5 vertices that maintains linear time performance and ensures all planar graphs have at most 5 colors.

As with the other algorithm implementations in the package, the new implementation does extensive error checking.  The results were checked on all graphs with 11 or fewer vertices in both debug and release modes on a Windows-based and an Intel-Linux computer.  In release mode, the results for 11 vertex graphs took just over 32000 seconds on a 2.4GHz CPU.

# Details #

This section reports the results of executing the vertex coloring method on all 11 vertex graphs.  For each graph coloring, the result is checked to ensure the graph has not been corrupted and to ensure no two edge endpoints have the same color.
These results were obtained using the following command-line:

  * ./planarity.exe -gens -c 11

```
# Edges  # graphs    <=5 colors  > 5 colors
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
      9        1305        1305           0
     10        3664        3664           0
     11       10250       10250           0
     12       28259       28259           0
     13       75415       75415           0
     14      192788      192788           0
     15      467807      467806           1
     16     1069890     1069888           2
     17     2295898     2295891           7
     18     4609179     4609156          23
     19     8640134     8640059          75
     20    15108047    15107819         228
     21    24630887    24630212         675
     22    37433760    37431857        1903
     23    53037356    53032213        5143
     24    70065437    70052308       13129
     25    86318670    86287197       31473
     26    99187806    99117568       70238
     27   106321628   106176479      145149
     28   106321628   106045210      276418
     29    99187806    98703029      484777
     30    86318670    85532987      785683
     31    70065437    68876874     1188563
     32    53037356    51330533     1706823
     33    37433760    35067762     2365998
     34    24630887    21509799     3121088
     35    15108047    11509170     3598877
     36     8640134     5347190     3292944
     37     4609179     2182540     2426639
     38     2295898      810164     1485734
     39     1069890      278998      790892
     40      467807       92222      375585
     41      192788       29351      163437
     42       75415        8360       67055
     43       28259        2009       26250
     44       10250         426        9824
     45        3664          87        3577
     46        1305          19        1286
     47         467           3         464
     48         172           1         171
     49          67           0          67
     50          26           0          26
     51          11           0          11
     52           5           0           5
     53           2           0           2
     54           1           0           1
     55           1           0           1
Totals   1018997864   996557620    22440244
```