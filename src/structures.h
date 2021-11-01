// Copyright University of Adelaide Hull5 2021, Harry Flynn, Emma Deakin,
// Samuel Allan, Brodie Ebbs, James Roughan, Elle Roberts, Tristan Betterman,
// John Gehlert and Luna Hoffman
// Data structures common to multiple parts
#ifndef SRC_STRUCTURES_H_
#define SRC_STRUCTURES_H_

#include <iostream>
#include <cmath>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/register/point.hpp>


// Structure which contains the defintion of a 2d point
struct Point2 {
    double X;
    double Y;

    friend inline bool operator== (const Point2& a, const Point2& b) {
        return a.X == b.X && a.Y == b.Y;
    }

    friend inline bool operator!= (const Point2& a, const Point2& b) {
        return !(a == b);
    }

    friend inline bool operator< (const Point2& a, const Point2& b) {
        return a.X < b.X || (a.X == b.X && a.Y < b.Y);
    }
};

BOOST_GEOMETRY_REGISTER_POINT_2D(Point2, double, cs::cartesian, X, Y)

typedef boost::geometry::model::polygon<Point2> polygon;
typedef boost::geometry::model::multi_polygon<polygon> multi_polygon;
typedef polygon::ring_type ring;

struct Point3 {
    double X;
    double Y;
    double Z;

    Point3(double x, double y, double z) : X{x}, Y{y}, Z{z}{};

    friend inline bool operator== (const Point3& a, const Point3& b) {
        return a.X == b.X && a.Y == b.Y && a.Z == b.Z;
    }

    friend inline bool operator!= (const Point3& a, const Point3& b) {
        return !(a == b);
    }
};

enum class ProjectionPlane {
    XY,
    XZ,
    YZ,
};

#endif  // SRC_STRUCTURES_H_
