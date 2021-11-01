#include "./merge.h"
#include "./parser.h"
#include "./structures.h"

#include <chrono>
#include <iostream>
#include <fstream>
#include <algorithm>

int main(int c, char* argv[]) {

    if (c == 2) {
	std::cout << "Usage: " << argv[0] << " sensitivity polyfile1 polyfile2 ..." << std::endl;
	return 69;
    }

    multi_polygon polys;
    for (int i = 2; argv[i]; i++) {
	std::ifstream file{argv[i]};
	double x, y;
	polygon newpoly;
	while (file >> x >> y) {
	    boost::geometry::append(newpoly, Point2{x, y});
	}
	boost::geometry::correct(newpoly);
	polys.push_back(newpoly);
    }

    multi_polygon res = merge(polys, std::stod(argv[1]));

    for (const auto &p : res) {
	for (const auto &vert : p.outer()) {
	    std::cout << "(" << vert.X << "," << vert.Y << ") ";
	}
	std::cout << std::endl;
    }

    return 0;
}
