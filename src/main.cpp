// Copyright University of Adelaide Hull5 2021, Harry Flynn, Emma Deakin,
// Samuel Allan, Brodie Ebbs, James Roughan, Elle Roberts, Tristan Betterman,
// John Gehlert and Luna Hoffman
#include <vector>
#include <string>
#include <iostream>
#include "structures.h"
#include "hull.h"
#include "parser.h"
#include "merge.h"
#include "simplify.h"

int main(int argc, char *argv[]) {
    int project = std::stoi(argv[1]);
    double extract_sense = std::stod(argv[2]);
    double a_toll = std::stod(argv[3]);
    double merge_sense = std::stod(argv[4]);
    double eps = std::stod(argv[5]);

    std::vector<std::vector<Point3>> mesh = parse_obj(std::string{argv[6]});
    multi_polygon flattened_hull = flatten(mesh,
                            static_cast<ProjectionPlane>(project));
    multi_polygon hull = extract_hull(flattened_hull, extract_sense, a_toll);

    for (int i = 7; i < argc; i++) {
        std::vector<std::vector<Point3>> mesh = parse_obj(std::string{argv[i]});
        multi_polygon flattened_hull = flatten(mesh,
                                static_cast<ProjectionPlane>(project));

        multi_polygon polygons = extract_hull(
            flattened_hull,
            extract_sense,
            a_toll);

        hull.insert(hull.end(), polygons.begin(), polygons.end());
    }

    write_to_file(hull, "extracted_hulls.txt");
    // std::cout << "..." << std::endl;

    multi_polygon merged_hulls = merge(hull, merge_sense);

    write_to_file(merged_hulls, "merged_hulls.txt");

    for (auto &poly : merged_hulls) {
        // std::vector<Point2> temp;
        // for (auto &vert : poly.outer()) {
        //     std::cout << vert.X << " " << vert.Y << std::endl;
        //     temp.push_back(vert);
        // }
        // temp = ::simplify(temp, eps);
        poly = ::simplify(poly, eps);
    }

    write_to_file(merged_hulls, "simplified_hulls.txt");
}
