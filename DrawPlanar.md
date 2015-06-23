# Introduction #

A sample graph is given in adjacency list format.  The two possible result files from the [planar graph drawing algorithm](http://www.springerlink.com/content/y13n582714695305/) are then shown.  The first file gives the combinatorial planar embedding followed by a special section for the visibility representation.  The second file is a character art rendition of the visibility representation.  This rendtion uses mn characters, so it is produced by a separate optional API call.

# Details #

The adjacency list format for the sample input graph is as follows:

```
N=15
0: 1 3 14 -1
1: 2 11 0 5 8 -1
2: 5 11 1 -1
3: 0 4 7 -1
4: 6 10 3 12 -1
5: 9 2 1 -1
6: 13 4 -1
7: 3 10 14 -1
8: 1 13 -1
9: 14 5 -1
10: 7 4 12 -1
11: 1 2 -1
12: 10 4 13 -1
13: 8 6 12 -1
14: 0 9 7 -1
```

The result of 'gp\_Embed()' with the DrawPlanar algorithm extension is below.  A _rotation scheme_ is imparted to the adjacency list of each vertex, resulting in a combinatorial planar embedding, and then a _visibility representation_ is produced and stored in a specially marked section of the output adjacency list file.  The DrawPlanar extension is also able to read the _visibility representation_ back in from this file format.

A _visibility representation_ is also sometimes called a _horvert_ diagram because it assigns a horizontal position and vertical length to each edge as well as a vertical position and horizontal length to each vertex.  Another way to look at this is that each vertex is placed in a vertical position and spans a horizontal range such that, for each vertex neighbor, there is an unobstructed vertical pathway along which the neighbor vertex can be seen.  The edge between the vertex and its neighbor is embedded along that unobstructed pathway.

```
N=15
0: 1 3 14 -1
1: 2 11 5 8 0 -1
2: 5 11 1 -1
3: 0 4 7 -1
4: 6 12 10 3 -1
5: 9 1 2 -1
6: 13 4 -1
7: 3 10 14 -1
8: 1 13 -1
9: 14 5 -1
10: 7 4 12 -1
11: 1 2 -1
12: 10 4 13 -1
13: 8 12 6 -1
14: 0 7 9 -1
<DrawPlanar>
0: 0 0 21
1: 14 4 21
2: 12 3 5
3: 1 8 11
4: 2 11 16
5: 11 2 7
6: 3 16 17
7: 8 9 13
8: 7 19 20
9: 10 1 2
10: 6 12 15
11: 13 5 6
12: 5 14 18
13: 4 17 19
14: 9 0 10
30: 21 0 14
31: 21 0 14
32: 8 0 1
33: 8 0 1
34: 0 0 9
35: 0 0 9
36: 4 12 14
37: 4 12 14
38: 6 13 14
39: 6 13 14
40: 7 11 14
41: 7 11 14
42: 20 7 14
43: 20 7 14
44: 3 11 12
45: 3 11 12
46: 5 12 13
47: 5 12 13
48: 11 1 2
49: 11 1 2
50: 9 1 8
51: 9 1 8
52: 16 2 3
53: 16 2 3
54: 12 2 6
55: 12 2 6
56: 14 2 5
57: 14 2 5
58: 2 10 11
59: 2 10 11
60: 17 3 4
61: 17 3 4
62: 13 6 8
63: 13 6 8
64: 10 8 9
65: 10 8 9
66: 19 4 7
67: 19 4 7
68: 1 9 10
69: 1 9 10
70: 15 5 6
71: 15 5 6
72: 18 4 5
73: 18 4 5
</DrawPlanar>
```

Given the _visibility representation_, it is very easy coding to produce the following character art rendition of the planar graph:

```
----------0-----------
|       |            |
|       -3--         |
|        | |         |
|        | --4---    |
|        |  | | |    |
|        |  | | 6-   |
|        |  | |  |   |
|        |  | |  -13 |
|        |  | |   || |
|        |  | --12-| |
|        |  |  |   | |
|        |  -10-   | |
|        |   |     | |
|        |   |     8-|
|        |   |      ||
|        --7--      ||
|         |         ||
-----14----         ||
 |                  ||
 9-                 ||
  |                 ||
  --5---            ||
   |   |            ||
   -2- |            ||
    || |            ||
    |11|            ||
    | ||            ||
    --------1---------
```