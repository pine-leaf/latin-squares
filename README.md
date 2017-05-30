# latin-squares
# latin-squares
For OSX, compile with:
g++-7 -O ls_main.cpp -o ls_main.o -fopenmp --std=c++11

You don't really need to use openmp, since it's not really used for much with
the current version. It was used in an earlier version which built up all of
the Transversals, saved them to memory and then used them to construct a
Latin Square. However, for larger Latin Squares, the number of Transversals
can grow too large and eat up too much memory.

build test input with:

python build_LS.py

./ls_main.o
