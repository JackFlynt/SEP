// Copyright University of Adelaide Hull5 2021, Harry Flynn, Emma Deakin,
// Samuel Allan, Brodie Ebbs, James Roughan, Elle Roberts, Tristan Betterman,
// John Gehlert and Luna Hoffman
#include "./hull.h"

#include <cstdlib>
#include <vector>
#include <utility>
#include <cmath>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <memory>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/geometries.hpp>

#include "structures.h"

multi_polygon flatten(
        const std::vector<std::vector<Point3>> &mesh,
        ProjectionPlane p
) {
    multi_polygon res;
    res.reserve(mesh.size());

    for (const auto &tri : mesh) {
        polygon new_tri;
        new_tri.outer().reserve(tri.size());
        for (const auto &point : tri) {
            if (p == ProjectionPlane::XY) {
                new_tri.outer().push_back({point.X, point.Y});
            } else if (p == ProjectionPlane::XZ) {
                new_tri.outer().push_back({point.X, point.Z});
            } else if (p == ProjectionPlane::YZ) {
                new_tri.outer().push_back({point.Y, point.Z});
            }
        }
        res.push_back(std::move(new_tri));
    }
    return res;
}

// Takes a point and return a value based on whioch side of the edge defined
// by p1 and p2 that point is on. Value is found from dot product of the normal
// and the vector from the lower "valued" point and the test point.
int check_edge_value(Point2 p, Point2 p1, Point2 p2) {
    if (p2 < p1) {  // Ensure p1 is lower value point.
        Point2 pT = p2;
        p2 = p1;
        p1 = pT;
    }

    double val = (p.X - p1.X)*(p2.Y - p1.Y) - (p.Y - p1.Y)*(p2.X - p1.X);
    if (val > 0) return 1;
    if (val < 0) return -1;
    return 0;
}

// Creates a point from an edge va;ue and an edge.
Point2 point_from_edge_value(int value, Point2 p1, Point2 p2, double eps
                                                                    = 0.00001) {
    if (p2 < p1) {  // Ensure p1 is lower value point.
        Point2 pT = p2;
        p2 = p1;
        p1 = pT;
    }

    double dx = (p2.Y-p1.Y);
    double dy = -(p2.X-p1.X);

    double x_mid = (p1.X+p2.X)/2.0;
    double y_mid = (p1.Y+p2.Y)/2.0;
    Point2 p;
    p.X = x_mid + static_cast<double>(value)*(eps*dx);
    p.Y = y_mid + static_cast<double>(value)*(eps*dy);

    return p;
}

// Traverses the edge of the mesh graph, removing any internal edges as it goes.
std::vector<Point2> traverse(Point2 root, Point2 p, std::unordered_map<Point2,
                                          std::vector<std::shared_ptr<edge>>, hash_fn> *graph) {
    std::vector<std::shared_ptr<edge>> *edges = &(*graph)[p];
    std::vector<std::shared_ptr<edge>> *tempEdges;
    Point2 temp;
    for (int i = 0; i < edges->size(); i++) {
        if ((*edges)[i]->internal) {
            // Erase internal edges
            temp = (*edges)[i]->other(p);
            tempEdges = &(*graph)[temp];

            tempEdges->erase(std::find(tempEdges->begin(), tempEdges->end(),
                        (*edges)[i]));

            edges->erase(edges->begin()+i);
            i--;
        } else {
            temp = (*edges)[i]->other(p);
            tempEdges = &(*graph)[temp];

            tempEdges->erase(std::find(tempEdges->begin(), tempEdges->end(),
                (*edges)[i]));

            edges->erase(edges->begin()+i);
            i--;

            if (root != p) {
                std::vector<Point2> temp_poly = traverse(root, temp, graph);
                temp_poly.push_back(p);
                return temp_poly;
            } else {
                return {p};
            }
        }
    }
    return {root, p};
}

// Simplify a mesh into a psueodohull.
std::unordered_map<Point2, std::vector<std::shared_ptr<edge>>, hash_fn> simplify_mesh(
                    multi_polygon mesh, double sensitivity) {
    /*  For each polygon in mesh:
            For each vertex in polygon:
                Check if vertex is in map:
                    Yes:
                        For each edge:
                            Check if edge already exists. If so check for
                            internel/external edge and remove internal edge
                    No:
                        Add vertex to map and record the connected edges */

    std::unordered_map<Point2, std::vector<std::shared_ptr<edge>>, hash_fn> graph;

    // Loop through mesh
    for (auto &poly : mesh) {
        auto &tri = poly.outer();
        // Round off point values
        if (sensitivity != 0.0) {
            tri[0].X = std::round(tri[0].X*sensitivity)/sensitivity;
            tri[0].Y = std::round(tri[0].Y*sensitivity)/sensitivity;
            tri[1].X = std::round(tri[1].X*sensitivity)/sensitivity;
            tri[1].Y = std::round(tri[1].Y*sensitivity)/sensitivity;
            tri[2].X = std::round(tri[2].X*sensitivity)/sensitivity;
            tri[2].Y = std::round(tri[2].Y*sensitivity)/sensitivity;
        }
        if (!(tri[0] == tri[1] || tri[0] == tri[2] || tri[1] == tri[2])) {
            // Check number of points which aleady exist.
            bool v1 = (graph.count(tri[0]) == 1);
            bool v2 = (graph.count(tri[1]) == 1);
            bool v3 = (graph.count(tri[2]) == 1);
            int count = graph.count(tri[0]) + graph.count(tri[1])
                        + graph.count(tri[2]);

            if (count == 0) {  // Add three new points and their edges
                auto e1 = std::make_shared<edge>(tri[0], tri[1], tri[2]);
                auto e2 = std::make_shared<edge>(tri[0], tri[2], tri[1]);
                auto e3 = std::make_shared<edge>(tri[1], tri[2], tri[0]);
                // edge *e1 = new edge(tri[0], tri[1], tri[2]);
                // edge *e2 = new edge(tri[0], tri[2], tri[1]);
                // edge *e3 = new edge(tri[1], tri[2], tri[0]);
                graph.insert({tri[0], {e1, e2}});
                graph.insert({tri[1], {e1, e3}});
                graph.insert({tri[2], {e2, e3}});

            } else if (count == 1) {  // Add two new points and their edges.
                int i = 0;
                int j = 1;
                int k = 2;


                // Ensure i is always existing point
                if (v2) {
                    i = 1;
                    j = 0;
                } else if (v3) {
                    i = 2;
                    k = 0;
                }
                // else v1

                // edge *e1 = new edge(tri[i], tri[j], tri[k]);
                // edge *e2 = new edge(tri[i], tri[k], tri[j]);
                // edge *e3 = new edge(tri[j], tri[k], tri[i]);
                auto e1 = std::make_shared<edge>(tri[i], tri[j], tri[k]);
                auto e2 = std::make_shared<edge>(tri[i], tri[k], tri[j]);
                auto e3 = std::make_shared<edge>(tri[j], tri[k], tri[i]);
                graph.insert({tri[j], {e1, e3}});
                graph.insert({tri[k], {e2, e3}});

                graph[tri[i]].push_back(e1);
                graph[tri[i]].push_back(e2);
            } else if (count == 2) {
                // Add one new point and its edges. Check existing edge.
                int i = 0;
                int j = 1;
                int k = 2;

                // Ensure i is always new point
                if (!v2) {
                    i = 1;
                    j = 0;
                } else if (!v3) {
                    i = 2;
                    k = 0;
                }
                // else !v1

                // edge *e1 = new edge(tri[i], tri[j], tri[k]);
                // edge *e2 = new edge(tri[i], tri[k], tri[j]);

                auto e1 = std::make_shared<edge>(tri[i], tri[j], tri[k]);
                auto e2 = std::make_shared<edge>(tri[i], tri[k], tri[j]);
                graph.insert({tri[i], {e1, e2}});
                graph[tri[j]].push_back(e1);
                graph[tri[k]].push_back(e2);

                std::vector<std::shared_ptr<edge>> *edges = &graph[tri[j]];

                // Check if edge is internal by comparing its value to the new
                // value.
                for (unsigned int r = 0; r < edges->size(); r++) {
                    if ((*edges)[r]->contains(tri[k])) {
                        if (check_edge_value(tri[i], tri[j], tri[k]) !=
                                                    (*edges)[r]->value) {
                            if ((*edges)[r]->value == 0) {
                                (*edges)[r]->value =
                                    check_edge_value(tri[i], tri[j], tri[k]);
                            } else if (check_edge_value(tri[i], tri[j], tri[k])
                                                                        != 0) {
                                (*edges)[r]->internal = true;
                            }
                        }
                        break;
                    }
                }

            } else if (count == 3) {
                // Check all three existing edges

                int i = 0;
                int j = 1;
                int k = 2;
                int temp;

                // Loop over all points
                for (int n = 0; n < 3; n++) {
                    std::vector<std::shared_ptr<edge>> *edges = &graph[tri[i]];
                    bool found = false;

                    // Check if edge is internal by comparing its value to the
                    // new value.
                    for (unsigned int r = 0; r < edges->size(); r++) {
                        if ((*edges)[r]->contains(tri[j])) {
                            if (check_edge_value(tri[k], tri[i], tri[j]) !=
                                                        (*edges)[r]->value) {
                                if ((*edges)[r]->value == 0) {
                                    (*edges)[r]->value =
                                    check_edge_value(tri[k], tri[i], tri[j]);
                                } else if (check_edge_value(tri[k], tri[i],
                                                                tri[j]) != 0) {
                                    (*edges)[r]->internal = true;
                                }
                            }
                            found = true;
                            break;
                        }
                    }

                    // If edge wasn't found add new edge.
                    if (!found) {
                        auto e1 = std::make_shared<edge>(tri[i], tri[j], tri[k]);
                        graph[tri[i]].push_back(e1);
                        graph[tri[j]].push_back(e1);
                    }

                    temp = k;
                    k = j;
                    j = i;
                    i = temp;
                }
            }
        } else if (!(tri[0] == tri[1] && tri[0] == tri[2] &&
                                            tri[1] == tri[2])) {
            int i = 0;
            int j = 1;

            if (tri[0] == tri[1]) {
                i = 0;
                j = 2;
            }

            bool v1 = (graph.count(tri[i]) == 1);
            bool v2 = (graph.count(tri[j]) == 1);
            int count = graph.count(tri[i]) + graph.count(tri[j]);

            if (count == 2) {
                std::vector<std::shared_ptr<edge>> *edges = &graph[tri[i]];
                bool found = false;
                for (unsigned int r = 0; r < edges->size(); r++) {
                    if ((*edges)[r]->contains(tri[j])) {
                        found = true;
                        break;
                    }
                }

                // If edge wasn't found add new edge.
                if (!found) {
                    auto e1 = std::make_shared<edge>(tri[i], tri[j], 0);
                    graph[tri[i]].push_back(e1);
                    graph[tri[j]].push_back(e1);
                }

            } else if (count == 1) {
                auto e1 = std::make_shared<edge>(tri[i], tri[j], 0);
                if (v1) {
                    graph.insert({tri[j], {e1}});
                    graph.at(tri[i]).push_back(e1);
                } else {
                    graph.insert({tri[i], {e1}});
                    graph.at(tri[j]).push_back(e1);
                }
            } else {
                auto e1 = std::make_shared<edge>(tri[i], tri[j], 0);
                graph.insert({tri[i], {e1}});
                graph.insert({tri[j], {e1}});
            }
        }
    }

    return graph;
}

multi_polygon extract_polygon(std::unordered_map
        <Point2, std::vector<std::shared_ptr<edge>>, hash_fn> graph, double a_toll, double point_dist) {
    /* Polygon extraction:
        Loop through each vertex:
            Loop though each edge:
                if edge is internal:
                    remove
                else
                    add vertex to new polygon as root
                    traverse edge to next vertex
                    Loop though each edge:
                        if edge is internal:
                            remove
                        else
                            add vertex to polygon
                            traverse edge to next vertex */

    Point2 temp;
    Point2 temp2;
    std::vector<std::vector<Point2>> polygons;
    std::vector<std::shared_ptr<edge>> *tempEdges;
    std::vector<std::shared_ptr<edge>> first_edge;

    std::unordered_map<Point2, std::vector<std::shared_ptr<edge>>, hash_fn> graph_stored
                                                                        = graph;

    // Loop through all verticies and extract the external edges.
    for ( auto it = graph.begin(); it != graph.end(); ++it ) {
        for (int i = 0; i < it->second.size(); i++) {
            if (it->second[i]->internal) {
                // Erase internal edges
                temp = it->second[i]->other(it->first);

                tempEdges = &graph.at(temp);
                tempEdges->erase(std::find(tempEdges->begin(), tempEdges->end(),
                    it->second[i]));

                it->second.erase(it->second.begin()+i);
                i--;
            } else {
                temp = it->first;
                temp2 = it->second[i]->other(it->first);

                tempEdges = &graph.at(temp2);
                first_edge.push_back(it->second[i]);
                tempEdges->erase(std::find(tempEdges->begin(), tempEdges->end(),
                    it->second[i]));

                it->second.erase(it->second.begin()+i);
                i--;

                polygons.push_back(traverse(temp, temp2, &graph));
            }
        }
    }

    std::vector<polygon> ls;
    std::vector<bool> bounding;
    Point2 check_point;

    for (int i = 0; i < polygons.size(); i++) {
        if (polygons[i].size() > 2) {
            polygon temp;
            for (const auto &vert : polygons[i]) {
                boost::geometry::append(temp, vert);
            }

            boost::geometry::correct(temp);

            bool external = false;
            for (int j = 1; j < polygons[i].size(); j++) {
                if (polygons[i][j-1] != polygons[i][j]) {
                    auto edges = &graph_stored[polygons[i][j-1]];
                    int edge_val = 0;
                    bool found = false;
                    for (unsigned int r = 0; r < edges->size(); r++) {
                        if ((*edges)[r]->contains(polygons[i][j])) {
                            edge_val = (*edges)[r]->value;
                        }
                    }

                    check_point = point_from_edge_value(edge_val,
                                polygons[i][j-1], polygons[i][j], point_dist);

                    if (boost::geometry::within(check_point, temp)) {
                        external = true;
                        break;
                    }
                }
            }

            if (external) {
                bounding.push_back(true);
            } else {
                bounding.push_back(false);
            }
            ls.push_back(temp);
        }
    }

    // Sort polygons by area
    for (int i = 0; i < ls.size(); i++) {
        double min = boost::geometry::area(ls[i]);
        int min_j = i;
        for (int j = i + 1; j < ls.size(); j++) {
            if (boost::geometry::area(ls[j]) < min){
                min = boost::geometry::area(ls[j]);
                min_j = j;
            }
        }

        polygon temp = ls[i];
        bool temp_bounding = bounding[i];
        ls[i] = ls[min_j];
        bounding[i] = bounding[min_j];
        ls[min_j] = temp;
        bounding[min_j] = temp_bounding;
    }

    // Remove any polygons which are below the area toll.
    for (int i = 0; i < ls.size(); i++) {
        if (boost::geometry::area(ls[i]) <= a_toll) {
            ls.erase(ls.begin() + i);
            bounding.erase(bounding.begin() + i);
            i -= 1;
        }
    }

    std::reverse(ls.begin(), ls.end());
    std::reverse(bounding.begin(), bounding.end());


    multi_polygon ls_out;
    multi_polygon temp_poly;
    for (int i = 0; i < ls.size(); i++) {
        if (bounding[i]) {
            if (!boost::geometry::within(ls[i], ls_out)) {
                boost::geometry::union_(ls[i], ls_out, temp_poly);
                ls_out = temp_poly;
                boost::geometry::clear(temp_poly);
            }
        } else {
            boost::geometry::difference(ls_out, ls[i], temp_poly);
            ls_out = temp_poly;
            boost::geometry::clear(temp_poly);
        }
    }
    boost::geometry::remove_spikes(ls_out);

    return ls_out;
}

// Extract hull from a polygon mesh.
multi_polygon extract_hull(multi_polygon mesh, double sensitivity, double a_toll,
                                                            double point_dist) {
    // Extract simplified mesh from original polygon mesh
    std::unordered_map<Point2, std::vector<std::shared_ptr<edge>>, hash_fn> graph =
                                            simplify_mesh(mesh, sensitivity);

    // Extract polygons and form hull
    return extract_polygon(graph, a_toll, point_dist);
}
