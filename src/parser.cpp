// Copyright University of Adelaide Hull5 2021, Harry Flynn, Emma Deakin,
// Samuel Allan, Brodie Ebbs, James Roughan, Elle Roberts, Tristan Betterman,
// John Gehlert and Luna Hoffman
// Impliments functions to allow obj parsing
#include "./parser.h"

#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include "./structures.h"

// Parses a obj file to a format simular to the original C# input
std::vector<std::vector<Point3>> parse_obj(std::string file) {
    std::vector<std::vector<Point3>> mesh;

    std::ifstream infile(file);

    std::string line;
    std::vector<Point3> vert;
    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        std::string str;
        iss >> str;
        if (str == "v") {
            iss >> str;
            double x = std::stod(str);
            iss >> str;
            double y = std::stod(str);
            iss >> str;
            double z = std::stod(str);
            vert.push_back(Point3(x, y, z));
        } else if (str == "f") {
            iss >> str;
            int v1 = std::stoi(str.substr(0, str.find('/')));
            iss >> str;
            int v2 = std::stoi(str.substr(0, str.find('/')));
            iss >> str;
            int v3 = std::stoi(str.substr(0, str.find('/')));
            mesh.push_back({vert[v1-1], vert[v2-1], vert[v3-1]});
        }
    }

    return mesh;
}

void write_to_file(multi_polygon hull, std::string file) {
    std::ofstream file_stream;
    file_stream.open(file);
    for (const auto &poly : hull) {
        for (const auto &vert : poly.outer()) {
            // std::cout << "(" << vert.X << "," << vert.Y << ") ";
            file_stream << "(" << vert.X << "," << vert.Y << ") ";
        }
        file_stream << std::endl;
        // std::cout << std::endl;
    }

    for (const auto &poly : hull) {
        for (const auto &p : poly.inners()) {
            for (const auto &vert : p) {
                // std::cout << "(" << vert.X << "," << vert.Y << ") ";
                file_stream << "(" << vert.X << "," << vert.Y << ") ";
            }
        }
        file_stream << std::endl;
        // std::cout << std::endl;
    }
    file_stream.close();
}
