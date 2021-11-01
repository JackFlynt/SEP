// Copyright University of Adelaide Hull5 2021, Harry Flynn, Emma Deakin,
// Samuel Allan, Brodie Ebbs, James Roughan, Elle Roberts, Tristan Betterman,
// John Gehlert and Luna Hoffman
//
// merging nearby polygons
#ifndef SRC_MERGE_H_
#define SRC_MERGE_H_

#include "./structures.h"

multi_polygon merge(multi_polygon polys, double sensitivity);

#endif  // SRC_MERGE_H_
