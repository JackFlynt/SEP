// Copyright University of Adelaide Hull5 2021, Harry Flynn, Emma Deakin,
// Samuel Allan, Brodie Ebbs, James Roughan, Elle Roberts, Tristan Betterman,
// John Gehlert and Luna Hoffman
#include <cmath>
#include <iostream>
#include "./simplify.h"

// Perpendicular distance of point from a line connecting p1 and p2
double perpendicular_distance(Point2 point, Point2 p1, Point2 p2) {
    // Implements https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line#Line_defined_by_two_points
if (p1 == p2) {
        return std::sqrt(pow(p2.X - point.X, 2) + pow(p2.Y - point.Y, 2));
    }
    double abs = std::abs((p2.X - p1.X) * (p1.Y - point.Y) - (p1.X - point.X)
                                                            * (p2.Y - p1.Y));
    double root = std::sqrt(pow(p2.X - p1.X, 2) + pow(p2.Y - p1.Y, 2));
    return abs / root;
}

polygon simplify(polygon input, double epsilon) {
    const auto s = simplify(input.outer(), epsilon);
    input.outer() = ring(begin(s), end(s));
    for (auto &r : input.inners()) {
        const auto s = simplify(r, epsilon);
        r = ring(begin(s), end(s));
    }
    return input;
}

std::vector<Point2> simplify(std::vector<Point2> input, double epsilon) {
    // Implements https://en.wikipedia.org/wiki/Ramer%E2%80%93Douglas%E2%80%93Peucker_algorithm

    /*  For points in input between start and end of line:
            Determine point that is most distant from the line [start, end]
            If most distant point is further away than epsilon:
                Recurse function on intervals [start, distant_point] and [distant_point, end]
                Return combined output from each interval
            Else:
                Return [start, end], deleting all points in between from the result */

    double max_distance = -1;
    int index = -1;

    for (int i = 1; i < input.size() - 1; i++) {
        double dist = perpendicular_distance(input[i], input[0],
                                                    input[input.size() - 1]);

        if (dist > max_distance) {
            max_distance = dist;
            index = i;
        }
    }

    if (max_distance > epsilon) {
        auto rec1 = simplify(std::vector<Point2>(input.begin(), input.begin()
                                                        + index + 1), epsilon);
        auto rec2 = simplify(std::vector<Point2>(input.begin()
                                                + index, input.end()), epsilon);
        rec1.insert(rec1.end(), rec2.begin() + 1, rec2.end());
        return rec1;
    }
    else {
        return { input[0], input[input.size() - 1] };
    }
}
