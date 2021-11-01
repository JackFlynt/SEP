// Copyright University of Adelaide Hull5 2021, Harry Flynn, Emma Deakin,
// Samuel Allan, Brodie Ebbs, James Roughan, Elle Roberts, Tristan Betterman,
// John Gehlert and Luna Hoffman
#ifndef HULL_H_
#define HULL_H_

#include <vector>
#include <memory>
#include <unordered_map>
#include "./structures.h"

int check_edge_value(Point2 p, Point2 p1, Point2 p2);

// Hash function for a 2d point.
struct hash_fn {
    std::size_t operator() (const Point2 point) const {
        std::size_t h1 = std::hash<double>()(point.X);
        std::size_t h2 = std::hash<double>()(point.Y);
        return (h1 ^ (h2 << 1));
    }
};

// Structure to represent an edge between two points. Also contians infomation
// about the orientation of attached polygons and if it it internal.
struct edge{
    Point2 p1;
    Point2 p2;
    int value;  // What original side polygon is on
    bool internal;

    edge(Point2 _p1, Point2 _p2, int _value) {
        p1 = _p1;
        p2 = _p2;

        if (p2 < p1) {  // Ensure p1 is lower value point.
            Point2 pT = p2;
            p2 = p1;
            p1 = pT;
        }

        value = _value;
        internal = false;
    }

    edge(Point2 _p1, Point2 _p2, Point2 vert) {
        p1 = _p1;
        p2 = _p2;

        if (p2 < p1) {  // Ensure p1 is lower value point.
            Point2 pT = p2;
            p2 = p1;
            p1 = pT;
        }

        value = check_edge_value(vert, p1, p2);
        internal = false;
    }

    bool contains(Point2 p) {
        return (p1 == p || p2 == p);
    }

    Point2 other(Point2 p) {
        if (p == p1) return p2;
        return p1;
    }
};

multi_polygon flatten(
        const std::vector<std::vector<Point3>> &mesh,
        ProjectionPlane p
);

std::unordered_map<Point2, std::vector<std::shared_ptr<edge>>, hash_fn> simplify_mesh(multi_polygon mesh, double sensitivity);

multi_polygon extract_polygon(std::unordered_map
                <Point2, std::vector<edge*>, hash_fn> graph, double a_toll,
                                                        double point_dist);

multi_polygon extract_hull(multi_polygon mesh, double sensitivity, double a_toll
                                        = 1.0, double point_dist = 0.00001);

#endif  // HULL_H_
