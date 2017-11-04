//
// Created by john on 10/17/17.
//

/// \file
/// \brief declaration of intersections::solve and related types

#ifndef INTERSECTIONS_H
#define INTERSECTIONS_H

#include <rectangle.h>

#include <unordered_map>
#include <vector>

namespace intersections {
    enum class Solution {
        simple,
        fast
    };

    // warning: contain non-owning pointers
    using RectangleSequence = std::vector<Rectangle const*>;

    using Intersections = std::unordered_map<Rectangle, RectangleSequence>;

    using Rectangles = std::vector<Rectangle>;

    // given a set of rectangles, return the map from overlap area to rectangles which overlap;
    // warning: returns non-owning pointers to input rectangles
    template<Solution>
    Intersections solve(Rectangles const& rectangles);
}

#endif //INTERSECTIONS_H
