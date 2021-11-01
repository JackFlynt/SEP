// Copyright University of Adelaide Hull5 2021, Harry Flynn, Emma Deakin,
// Samuel Allan, Brodie Ebbs, James Roughan, Elle Roberts, Tristan Betterman,
// John Gehlert and Luna Hoffman
// Defines fucntions for obj parsing and defines 2d and 3d point structs


#ifndef SRC_PARSER_H_
#define SRC_PARSER_H_

#include <vector>
#include <string>
#include "./structures.h"

std::vector<std::vector<Point3>> parse_obj(std::string file);
void write_to_file(multi_polygon hull, std::string file);

#endif  // SRC_PARSER_H_
