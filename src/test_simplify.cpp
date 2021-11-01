// Copyright University of Adelaide Hull5 2021, Harry Flynn, Emma Deakin,
// Samuel Allan, Brodie Ebbs, James Roughan, Elle Roberts, Tristan Betterman,
// John Gehlert and Luna Hoffman
// Tester program for function defined in simplify.h

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "simplify.h"

int main(int c, char* argv[]) {
    if (c < 3) {
        std::cout << "Please pass test input file name and epsilon as arguments"
                                                                << std::endl;
        return 69;
    }

    std::fstream input(argv[1]);
    double X, Y;
    std::vector<Point2> points;

    while (input >> X >> Y) {
        points.push_back({X, Y});
    }

    auto output = simplify(points, std::stod(argv[2]));
    for (auto point : output) {
        std::cout << point.X << " " << point.Y << std::endl;
    }

    return 0;
}
