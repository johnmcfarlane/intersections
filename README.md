# Rectangle Intersection

## Introduction

This project implements a rectangle intersection calculator written in C++14 with 
CMake project build configuration and using the 
[RapidJSON](https://github.com/Tencent/rapidjson) parsing library.

## Systems Requirements

Tested on Ubuntu 16.04 with:
* CMake 3.9.4
* GCC 5.4.0 and Clang 3.6

Tested on Windows 10 with:
* Microsoft Visual Studio Community 2017 Preview (15.4.0 Preview 2.0)

## Download

From the command line:

```bash
git clone --recursive https://github.com/johnmcfarlane/intersections.git
cd intersections
```

## Programs

Three CMake targets are defined.

### <a id="lib"></a>`intersections` Library Target

A library that exposes a solver through header file,

```c++
#include <intersections>
```

and function,

```c++
namespace intersections {
  template<Solution>
  Intersections solve(Rectangles const& rectangles);
}
```

which takes a vector of rectangles and returns a map of intersection area to
the set of overlapping rectangles.

For examples of how to invoke `solve`, see [*test.cpp*](src/test.cpp) in the 
[`tests`](#tests) target and [*main.cpp*](src/main.cpp) in the 
[`main`](#main) target.

### <a id="tests"></a>`tests` Binary Target

Run this binary to perform speed and correctness tests on the functionality of
the [`intersections`](#lib) library target.

```sh
cmake -DCMAKE_BUILD_TYPE=Release
make tests
./tests
```

Tests include:

* simple one-off tests;
* procedually-generated performance and correctness tests and
* stress tests involve input sets of increasing size.

Note that the program is not expected to complete on 32GB systems due to 
memory requirements. However, a single test involving 1024 rectangles 
should complete within an hour on a modern x86-64 system.

### <a id="main"></a>`main` Binary Target

`main` is a command-line utility that takes the filename of a JSON file as 
input and prints the intersections of rectangles contained in that file. The 
utility uses the [RapidJSON](https://github.com/Tencent/rapidjson) library to 
parse the content of the JSON file and the [`intersections`](#lib) library to 
find the intersections.

See sample JSON file, [rectangles.json](rectangles.json) for an example of the
file format.

To build the binary and run *rectangles.json* through it:

```sh
cmake -DCMAKE_BUILD_TYPE=Release
make main
./intersections rectangles.json
```

## Algorithms

Two algorithms with noteworthy properties are implemented: *simple* and 
*fast*. Their relative performance is plotted
[here](https://docs.google.com/spreadsheets/d/1Zr6UykZJTgoL-rxxyBQzMffEGO_T62k8K31dmDSbE9Y/edit?usp=sharing).
Tests involve rectangles with edges selected randomly in the range [0, 250], 
were compiled with gcc-7.2 and run in Linux on an Intel(R) Core(TM) i7-6700HQ 
CPU @ 2.60GHz.

### Simple

The *simple* algorithm can be found in the `solve<Solution::simple>` function 
in the [src/simple.cpp](src/simple.cpp) file. It takes a sequence of 
rectangles and returns a mapping from areas of overlap to the rectangles which
produce that overlapping area.

Each combination of rectangles is generated using recursive binary search and 
if that combination satisfies the following criteria, it is added to the map:

1. it must contain at least two rectangles;
2. the rectangles must overlap and
3. a result with the same overlap must not already exist in the map.

Each recursion of the binary search visits branches along which rectangles are
present *before* branches along which they are absent. Combined with 3), this
ensures that the most populous results for each overlap area are found first.
Each recursion also prunes branches which do not satisfy 2).

The *simple* algorithm is very fast at finding intersections with low numbers 
of rectangles. But by 50 rectangles, it is no longer the faster solution. The 
complexity of this algorithm is `(O)2^N` on the number of rectangles.

    rectangles  seconds     intersections
    2           1.58E-05    1
    3           5.76E-06    2
    4           5.17E-06    4
    6           5.57E-06    5
    8           6.63E-06    9
    12          1.68E-05    33
    16          2.14E-05    46
    24          6.95E-05    144
    32          0.000290494 431
    48          0.00607432  1548
    64          0.114225    3819
    96          52.9712     16437

### Fast

The *fast* algorithm can be found in the `solve<Solution::fast>` function in
the [src/fast.cpp](src/fast.cpp) file. It is more complicated than *simple* 
and is slower for ~50 rectangles or fewer. 

In essence, the *fast* algorithm works by sweeping across the horizontal and 
vertical ranges occupied by the rectangles and extracting the set rectangles 
that occupy those ranges. It then submits the intersection of the two sets as
a result.

This algorithm scales better than *simple* but still exhibits roughly (O)N^2 
complexity on the number of rectangles. That is to be expected as the size of 
the results grows at a yet-greater rate.

    rectangles  seconds        intersections
    2           4.40E-05       1
    3           4.68E-05       2
    4           4.17E-05       4
    6           4.34E-05       5
    8           9.09E-05       9
    12          0.000249754    33
    16          0.000206417    46
    24          0.00115523     144
    32          0.00156291     431
    48          0.0068363      1548
    64          0.024906       3819
    96          0.119216       16437
    128         0.325622       48674
    192         1.76884        226120
    256         4.72813        525437
    384         17.0733        1890879
    512         38.0947        4333026
    768         107.722        12367299
    1024        1208.89        27463837

## Future Directions

### Memory Safety

The solutions return results with non-owning pointers. If the input vector is
destroyed or resized before the results are read, those pointers are 
invalidated. Raw pointers could be replaced with `std::shared_ptr` but this 
would pessimize the solver. A better approach might be to add an extra 
step to convert from pointers to array indices.

### Better Associative Containers

In general, `std::unordered_set` and `std::unordered_map` are chosen for
associative containers and then relied upon heavily. `std::map` is used in one
instance where order matters. In all other cases, the `unordered_` containers
are either faster or as fast. However, there are better, non-standard 
solutions available which would speed up the algorithms. They were chosen 
against because they would add an extra dependency and would be unlikely to
reduce complexity.

### Improved Cache Locality

The *simple* algorithm is bound by its complexity. However, the *fast* 
algorithm suffers performance degredation when the scale of the problem 
reaches a certain size. This is likely due to the sheer number of results 
that are collected. There are two ways this situation could be improved:

1. The *fast* algorithm submits duplicate intersections. There may be a way to
   improve it to avoid duplicates. If that were the case, `std::vector` could 
   be used to collate the results and no lookup would be required.

2. Alternatively, results for single iterations of the outer loop could be
   maintained separately and merged into the overall set of results in a 
   single step.

### Parallelism

The algorithms could be sped up on multi-processor systems by parallelizing
them. For example, the outer loop of the *fast* algorithm could easily be
distributed across a pool of threads. Additionally, this might improve 
performance further by increasing cache locality as described above.
