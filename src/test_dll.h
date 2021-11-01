#include <cstddef>
struct Point2
{
    double X;
    double Y;
};

struct Point3
{
    double X;
    double Y;
    double Z;
};

struct Polygon
{
    Point2 *poly;
    size_t length;
};

struct PolygonList
{
    Polygon *list;
    size_t length;
};

struct MultiPolygon
{
    PolygonList *inner;
    PolygonList outer;
};

struct Point3List
{
    Point3 *list;
    size_t length;
};

struct Polyhedron
{
    Point3List *list;
    size_t length;
};