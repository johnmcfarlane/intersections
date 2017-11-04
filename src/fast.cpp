//
// Created by john on 11/1/17.
//

#include <intersections.h>

#include "transitions.h"

#include <numeric>
#include <set>

using namespace intersections;

namespace {
    // Given a set of rectangle edges aligned along an particular axis,
    // call the given function for combinations of rectangles that span a common range.
    template<typename Container, typename Transitions, typename Function>
    void for_each_range(Transitions const& transitions, Function function)
    {
        // For each position at which rectangle edges occur,
        Container opening_rectangles;
        auto horizontal_end = std::end(transitions);
        for (auto open_iterator = std::begin(transitions); open_iterator!=horizontal_end; ++open_iterator) {
            auto const& open = open_iterator->second;

            // remove rectangles with closing edges from the opening_rectangles set
            for (auto const ending_rectangle : open.ending) {
                opening_rectangles.erase(ending_rectangle);
            }

            // and for opening edges,
            if (!open.starting.empty()) {
                for (auto const starting_rectangle : open.starting) {
                    // add them to the set
                    opening_rectangles.insert(starting_rectangle);
                }

                // and then sweep through the remaining rectangle edges
                // and while there there are still multiple rectangles in the set,
                auto closing_rectangles = opening_rectangles;
                for (auto close_iterator = std::next(open_iterator);
                     closing_rectangles.size()>=2;
                     ++close_iterator) {
                    assert(close_iterator!=horizontal_end);

                    // then for rectangles with closing edges,
                    auto const& close = close_iterator->second;
                    if (!close.ending.empty()) {
                        // call the given function object.
                        function(closing_rectangles);

                        // Remove rectangles with closing edges from the opening_rectangles set.
                        for (auto const ending_rectangle : close.ending) {
                            closing_rectangles.erase(ending_rectangle);
                        }
                    }
                }
            }
        }
        assert(opening_rectangles.empty());
    }
}

namespace intersections {
    template<>
    Intersections solve<Solution::fast>(Rectangles const& rectangles)
    {
        assert(std::all_of(std::begin(rectangles), std::end(rectangles), is_positive));

        Intersections output;

        auto const horizontal_transitions = make_transitions<Axis::horizontal>(rectangles);

        // For each horizontal range,
        for_each_range<Transitions<Axis::vertical>>(
                horizontal_transitions,
                [&output](auto const& vertical_transitions) {

                    // for each vertical sub-range,
                    for_each_range<std::set<Rectangle const*>>(
                            vertical_transitions,
                            [&output](auto const& constituents) {

                                // calculate the overlapping area
                                auto overlap = std::accumulate(
                                        std::begin(constituents), std::end(constituents),
                                        maximum_rectangle, [](auto accumulation, auto const* rectangle) {
                                            return accumulation & *rectangle;
                                        });
                                assert(is_positive(overlap));

                                // and look up to see if it is represented in the results.
                                auto found = output.find(overlap);
                                if (found!=std::end(output)) {
                                    // If it is already represented, it should be consistent.
                                    assert(std::equal(
                                            std::begin(found->second), std::end(found->second),
                                            std::begin(constituents), std::end(constituents)));
                                }
                                else {
                                    // If it is not alread represented, add it.
                                    output.emplace(
                                            overlap,
                                            RectangleSequence(std::begin(constituents), std::end(constituents)));
                                }
                            });
                });

        return output;
    }
}
