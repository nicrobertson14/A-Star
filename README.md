# A-Star

This code is an implementation of A Star Search, finding the shortest path between 2 points on a map.


## Compiling
To compile run ```g++ -o Search Search.cpp```

## Testing/Usage
Provide a .txt file containing map dimensions and a grid of space-separated 0s and 1s (see /tests for examples), as well as x y start and goal coordinates as follows:
```
./Search ./tests/map.txt 0 0 3 7
```

This returns the shortest path from 0,0 to 3,7: ```R D D D D D R R R D D L```
