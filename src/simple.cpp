//
// Created by john on 10/20/17.
//

/// \file
/// \brief defines intersections::solve and supporting functions and types

#include <intersections.h>

#include <functional>

using namespace intersections;

namespace {
    using RectanglesIterator = typename Rectangles::const_iterator;
    using RectangleSequence = std::vector<Rectangle const*>;

    void submit(Intersections& intersections, RectangleSequence const& constituents, Rectangle const overlap)
    {
        auto const found = intersections.find(overlap);

        // If this area of overlap is already represented,
        if (found!=std::end(intersections)) {
            // then it's by a super-set of rectangles
            assert(std::includes(
                    std::begin(found->second), std::end(found->second),
                    std::begin(constituents), std::end(constituents)));

            // that's greater.
            assert(std::distance(std::begin(found->second), std::end(found->second))
                    >int(constituents.size()));

            return;
        }

        // Otherwise, add it.
        intersections.emplace(overlap, constituents);
    }

    // helper function for intersections::combinations
    void recurse(
            RectanglesIterator const first, RectanglesIterator const last,
            RectangleSequence& constituents, Rectangle const overlap,
            Intersections& intersections)
    {
        auto const remaining = std::distance(first, last);

        // leaf condition
        if (remaining==0) {
            if (constituents.size()>=2) {
                submit(intersections, constituents, overlap);
            }
            return;
        }

        auto const next = std::next(first);

        // recurse with rectangle included
        auto const& first_rectangle = *first;
        auto const next_overlap = overlap & first_rectangle;
        if (is_positive(next_overlap)) {
            constituents.push_back(&first_rectangle);
            recurse(next, last, constituents, next_overlap, intersections);
            constituents.pop_back();
        }

        // recurse with rectangle excluded
        recurse(next, last, constituents, overlap, intersections);
    }
}

namespace intersections {
    template<>
    Intersections solve<Solution::simple>(Rectangles const& rectangles)
    {
        // all input rectangles must have positive area
        auto const first = std::begin(rectangles);
        auto const last = std::end(rectangles);
        assert(std::all_of(first, last, is_positive));

        RectangleSequence constituents;
        Intersections intersections;

        recurse(first, last, constituents, maximum_rectangle, intersections);

        return intersections;
    }
}
