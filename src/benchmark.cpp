// Copyright University of Adelaide Hull5 2021, Harry Flynn, Emma Deakin,
// Samuel Allan, Brodie Ebbs, James Roughan, Elle Roberts, Tristan Betterman,
// John Gehlert and Luna Hoffman
// Test program that runs each component on an input file and tracks time taken
// for each step
// Outputs results to filename specified in argv[3], or cout if not given

#include <chrono>
#include <iostream>
#include <fstream>
#include <ostream>

#include "./hull.h"
#include "./parser.h"
#include "./structures.h"
#include "./simplify.h"
#include "./merge.h"

using namespace std::chrono;

int main(int c, char* argv[]) {
    if (c < 3) {
        std::cout <<
        "Pass arguments: input_file poly_simplification_epsilon output_file?"
        << std::endl;
        return 69;
    }

    auto start_time = high_resolution_clock::now();
    auto object = parse_obj(argv[1]);
    auto parse_time = high_resolution_clock::now();
    auto flattened = flatten(object, ProjectionPlane::XY);
    auto flattened_time = high_resolution_clock::now();
    auto hull = extract_hull(flattened, 0.0);
    auto hull_time = high_resolution_clock::now();
    auto merged = merge(hull, 0.01);
    auto merge_time = high_resolution_clock::now();

    for (auto poly : merged) {
        simplify(poly, std::stod(argv[2]));
    }

    auto poly_simplify_time = high_resolution_clock::now();

    // Output to output, which is a file if filename specified, else cout
    std::ofstream _f;
    std::ostream & output = (c > 3) ? (_f.open(argv[3]), _f) : std::cout;
    output << "Interval to parse:            " <<
    ((parse_time - start_time) / milliseconds(1)) << " ms" << std::endl;
    output << "Interval to flatten:          " <<
    ((flattened_time - parse_time) / milliseconds(1)) << " ms" << std::endl;
    output << "Interval to simplify:         " <<
    ((hull_time - flattened_time) / milliseconds(1)) << " ms" << std::endl;
    output << "Interval for merge:           " <<
    ((merge_time - hull_time) / milliseconds(1)) << " ms" << std::endl;
    output << "Interval for poly simplify:   " <<
    ((poly_simplify_time - merge_time) / milliseconds(1)) << " ms" << std::endl;
    output << "Total time:                   " <<
    ((merge_time - start_time) / milliseconds(1)) << " ms" << std::endl;
    _f.close();
    return 0;
}
