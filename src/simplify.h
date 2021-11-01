// Copyright University of Adelaide Hull5 2021, Harry Flynn, Emma Deakin,
// Samuel Allan, Brodie Ebbs, James Roughan, Elle Roberts, Tristan Betterman,
// John Gehlert and Luna Hoffman
// Defines function to simplify input polygon (vector<Point2>) by removing
// points within epsilon range of being colinear

#ifndef SRC_SIMPLIFY_H_
#define SRC_SIMPLIFY_H_

#include "./structures.h"
#include <vector>

polygon simplify(polygon input, double epsilon);
std::vector<Point2> simplify(std::vector<Point2> input, double epsilon);

#endif  // SRC_SIMPLIFY_H_
