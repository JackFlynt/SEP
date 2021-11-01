// Copyright University of Adelaide Hull5 2021, Harry Flynn, Emma Deakin,
// Samuel Allan, Brodie Ebbs, James Roughan, Elle Roberts, Tristan Betterman,
// John Gehlert and Luna Hoffman
#include "structures.h"
#include <boost/geometry/geometry.hpp>

// merges adjacent polygons, allowing for a slight gap between them
// this method is slow and should only be used when you need to handle
// polys that may not touch, otherwise use boost union_ or the hull extraction
// methods
multi_polygon merge(multi_polygon polys, double sensitivity) {
    multi_polygon out;
    int points_per_circle = 36;
    boost::geometry::strategy::buffer::distance_symmetric<double> distance_strategy(sensitivity);
    boost::geometry::strategy::buffer::side_straight side_strategy;
    boost::geometry::strategy::buffer::join_miter join_strategy;
    boost::geometry::strategy::buffer::end_round end_strategy(points_per_circle);
    boost::geometry::strategy::buffer::point_circle circle_strategy(points_per_circle);

    // feather the edges out, this will merge nearby polygons
    boost::geometry::buffer(
            polys,
            out,
            distance_strategy,
            side_strategy,
            join_strategy,
            end_strategy,
            circle_strategy
    );

    // feather the edges back in
    boost::geometry::strategy::buffer::distance_symmetric<double> reverse_distance_strategy(-sensitivity);

    multi_polygon res;
    boost::geometry::buffer(
            out,
            res,
            reverse_distance_strategy,
            side_strategy,
            join_strategy,
            end_strategy,
            circle_strategy
    );
    return res;
}
