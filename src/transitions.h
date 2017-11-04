//
// Created by john on 10/31/17.
//

#ifndef INTERSECTIONS_TRANSITIONS_H
#define INTERSECTIONS_TRANSITIONS_H

#include <rectangle.h>

#include <map>
#include <unordered_set>

// enable prohibitively slow asserts
//#define THOROUGH_ASSERTS

#if defined(THOROUGH_ASSERTS)
#define AXIOM assert
#else
#define AXIOM(CONDITION) do {} while (false)
#endif

namespace intersections {
    // at a given horizontal or vertical position, these rectangles start or end
    struct TransitionMapped {
        using Set = std::unordered_set<Rectangle const*>;

        // the set of rectangles which end at this position
        Set ending;

        // the set of rectangles which start at this position
        Set starting;
    };

    // maps out all of the positions along an axis where a Rectangle begins or ends
    template<Axis axis>
    class Transitions {
    public:
        Transitions() = default;

        Transitions(Transitions const&) = default;

        Transitions(Transitions&& that)
        {
            using std::swap;
            swap(this->steps, that.steps);
            swap(this->num_rectangles, that.num_rectangles);
        }

        Transitions& operator=(Transitions const&) = default;

        Transitions& operator=(Transitions&& that)
        {
            using std::swap;
            swap(this->steps, that.steps);
            swap(this->num_rectangles, that.num_rectangles);
            return *this;
        }

        ~Transitions()
        {
            assert(valid());
        }

        auto empty() const noexcept
        {
            return size()==0;
        }

        auto size() const noexcept
        {
            return num_rectangles;
        }

        auto begin() const noexcept
        {
            return std::begin(steps);
        }

        auto end() const noexcept
        {
            return std::end(steps);
        }

        void clear() noexcept
        {
            steps.clear();
            num_rectangles = 0;
        }

        void insert(Rectangle const* rectangle)
        {
            AXIOM(valid());

            auto starting_insert_result = steps[rectangle->interval(axis).start].starting.insert(rectangle);
            auto ending_insert_result = steps[rectangle->interval(axis).end].ending.insert(rectangle);

            if (starting_insert_result.second!=ending_insert_result.second) {
                assert(false);
            }
            num_rectangles += starting_insert_result.second;

            AXIOM(valid());
        }

        void erase(Rectangle const* rectangle)
        {
            AXIOM(valid());

            auto starting_erase_result = steps[rectangle->interval(axis).start].starting.erase(rectangle);
            auto ending_erase_result = steps[rectangle->interval(axis).end].ending.erase(rectangle);

            if (starting_erase_result!=ending_erase_result) {
                assert(false);
            }
            num_rectangles -= int(starting_erase_result);

            AXIOM(valid());
        }

    private:
        // returns true iff class invariants hold
        auto valid() const noexcept
        {
            std::map<Rectangle const*, int> rectangle_edge_counts;
            std::map<Rectangle const*, int> rectangle_counts;

            for (auto const& step : steps) {
                for (auto const starting_rectangle : step.second.starting) {
                    ++rectangle_edge_counts[starting_rectangle];

                    auto& rectangle_count = rectangle_counts[starting_rectangle];
                    ++rectangle_count;
                    if (rectangle_count!=1) {
                        return false;
                    }
                }

                for (auto const ending_rectangle : step.second.ending) {
                    ++rectangle_edge_counts[ending_rectangle];

                    auto& rectangle_count = rectangle_counts[ending_rectangle];
                    --rectangle_count;
                    if (rectangle_count!=0) {
                        return false;
                    }
                }
            }

            if (rectangle_edge_counts.size()!=rectangle_counts.size()) {
                return false;
            }

            if (num_rectangles!=int(rectangle_counts.size())) {
                return false;
            }

            if (!std::all_of(
                    std::begin(rectangle_edge_counts), std::end(rectangle_edge_counts), [](auto rectangle_edge_count) {
                        return rectangle_edge_count.second==2;
                    })) {
                return false;
            }

            if (!std::all_of(
                    std::begin(rectangle_counts), std::end(rectangle_counts), [](auto rectangle_count) {
                        return rectangle_count.second==0;
                    })) {
                return false;
            }

            return true;
        }

        std::map<int, TransitionMapped> steps;
        int num_rectangles = 0;
    };

    template<Axis axis>
    auto make_transitions(Rectangles const& rectangles)
    {
        Transitions<axis> edges;
        for (auto const& rectangle : rectangles) {
            edges.insert(&rectangle);
        }
        return edges;
    }
}

#endif //INTERSECTIONS_TRANSITIONS_H
