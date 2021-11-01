#include "hull.h"
#include "merge.h"
#include "simplify.h"
#include "parser.h"
#define DllExport extern "C" __attribute__((dllexport, cdecl))

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

ring poly_to_ring(Polygon poly)
{
    ring new_ring = ring();
    for (int i = 0; i < poly.length; i++)
    {
        new_ring.push_back(poly.poly[i]);
    }
    return new_ring;
}

polygon poly_to_boost(Polygon poly)
{
    ring boost_ring = poly_to_ring(poly);
    polygon new_poly = polygon();
    new_poly.outer() = boost_ring;
    return new_poly;
};

Polygon boost_to_poly(polygon boost_poly)
{
    std::vector<Point2> *temp_poly = new std::vector<Point2>();
    for (const auto &val : boost_poly.outer())
    {
        temp_poly->push_back(val);
    }
    return Polygon{temp_poly->data(), temp_poly->size()};
};

multi_polygon multipoly_to_boost(MultiPolygon poly)
{
    multi_polygon new_multi = multi_polygon();
    for (int i = 0; i < poly.outer.length; i++)
    {
        ring outer_ring = poly_to_ring(poly.outer.list[i]);
        polygon::inner_container_type inner_rings;
        for (int j = 0; j < poly.inner[i].length; i++)
        {
            inner_rings.push_back(poly_to_ring(poly.inner[i].list[j]));
        }
        polygon new_poly = polygon();
        new_poly.outer() = outer_ring;
        new_poly.inners() = inner_rings;
        new_multi.push_back(new_poly);
    }
    return new_multi;
}

MultiPolygon boost_to_multipoly(multi_polygon boost_poly)
{
    std::vector<Polygon> *OuterPolygons = new std::vector<Polygon>();
    std::vector<PolygonList> *InnerPolygons = new std::vector<PolygonList>();

    for (const auto &vert : boost_poly)
    {
        std::vector<Point2> *temp_poly = new std::vector<Point2>();
        for (const auto &val : vert.outer())
        {
            temp_poly->push_back(val);
        }
        OuterPolygons->push_back(Polygon{temp_poly->data(), temp_poly->size()});
    }

    for (const auto &vert : boost_poly)
    {
        std::vector<Polygon> *temp_list = new std::vector<Polygon>;
        for (const auto &in : vert.inners())
        {
            std::vector<Point2> *temp_poly = new std::vector<Point2>();
            for (const auto &val : in)
            {
                temp_poly->push_back(val);
            }
            temp_list->push_back(Polygon{temp_poly->data(), temp_poly->size()});
        }

        InnerPolygons->push_back(PolygonList{temp_list->data(), temp_list->size()});
    }

    return MultiPolygon{
        InnerPolygons->data(),
        PolygonList{OuterPolygons->data(), OuterPolygons->size()}};
};

std::vector<std::vector<Point3>> polyhedron_to_vecs(Polyhedron poly)
{
    std::vector<std::vector<Point3>> vecs;
    for (int i = 0; i < poly.length; i++)
    {
        std::vector<Point3> vec(poly.list[i].list, poly.list[i].list + poly.list[i].length);
        vecs.push_back(vec);
    }
    return vecs;
};

Polyhedron vecs_to_polyhedron(std::vector<std::vector<Point3>> &vecs)
{
    std::vector<Point3List> *outer_vec = new std::vector<Point3List>;
    for (auto &vec : vecs)
    {
        outer_vec->push_back(Point3List{vec.data(), vec.size()});
    }
    return Polyhedron{outer_vec->data(), outer_vec->size()};
};

DllExport MultiPolygon extract_hull(MultiPolygon mesh, double sensitivity, double a_toll = 1.0, double point_dist = 0.00001)
{
    multi_polygon boost_mesh = multipoly_to_boost(mesh);
    multi_polygon boost_hull = ::extract_hull(boost_mesh, sensitivity, a_toll, point_dist);
    return boost_to_multipoly(boost_hull);
};
DllExport MultiPolygon merge(MultiPolygon polys, double sensitivty)
{
    multi_polygon boost_polys = multipoly_to_boost(polys);
    multi_polygon boost_merged = ::merge(boost_polys, sensitivty);
    return boost_to_multipoly(boost_merged);
};
DllExport Polygon simplify(Polygon input, double epsilon)
{
    polygon boost_input = poly_to_boost(input);
    polygon boost_output = ::simplify(boost_input, epsilon);
    return boost_to_poly(boost_output);
};
DllExport Polyhedron parse_obj(char *file)
{
    std::vector<std::vector<Point3>> vecs_obj = ::parse_obj(std::string(file));
    return vecs_to_polyhedron(vecs_obj);
};
DllExport MultiPolygon flatten(Polyhedron mesh, int p)
{
    std::vector<std::vector<Point3>> vecs_mesh = polyhedron_to_vecs(mesh);
    multi_polygon boost_flat = ::flatten(vecs_mesh, (ProjectionPlane)p);
    return boost_to_multipoly(boost_flat);
};

DllExport MultiPolygon suite(char *file1, char *file2, int len, double extract_sensitivity, double a_toll, double point_dist, double merge_sensitivity, double epsilon)
{
    multi_polygon hulls = ::extract_hull(::flatten(::parse_obj(std::string(file1)), (ProjectionPlane)0), extract_sensitivity, a_toll, point_dist);
    multi_polygon current_hull = ::extract_hull(::flatten(::parse_obj(std::string(file2)), (ProjectionPlane)0), extract_sensitivity, a_toll, point_dist);
    hulls.insert(hulls.end(), current_hull.begin(), current_hull.end());

    multi_polygon merged_hull = ::merge(hulls, merge_sensitivity);
    for (auto &vert : merged_hull)
    {
        vert = ::simplify(vert, epsilon);
    }
    return boost_to_multipoly(merged_hull);
}