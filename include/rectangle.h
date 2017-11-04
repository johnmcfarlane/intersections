//
// Created by john on 10/21/17.
//

/// \file
/// \brief definition of intersections::Rectangle and related functions and types

#ifndef INTERSECTIONS_RECTANGLE_H
#define INTERSECTIONS_RECTANGLE_H

#include "interval.h"

#include <array>
#include <cassert>
#include <limits>

namespace intersections {
    // Many multi-dimensional problems can be reduced to one dimension.
    // This enum helps ensure that they are not solved twice.
    enum class Axis {
        horizontal,
        vertical,
        size
    };

    // axis-aligned rectangle with integer coordinates;
    // represented as top-left corner and extent
    class Rectangle {
    public:
        constexpr Rectangle() noexcept = default;

        constexpr Rectangle(int x, int y, int w, int h) noexcept
                :intervals{Interval::from_extent(x, w), Interval::from_extent(y, h)}
        {
        }

        // Intervals, i.e. [start..end), are a better way to represent AABBs
        // but the interface calls for width and height
        // so to avoid confusion, intervals are exposed via named functions.
        static constexpr Rectangle from_intervals(Interval horizontal, Interval vertical) noexcept
        {
            Rectangle r;
            r.intervals[0] = horizontal;
            r.intervals[1] = vertical;
            return r;
        }

        constexpr Interval const& interval(Axis axis) const noexcept
        {
            return intervals[int(axis)];
        }

        constexpr auto x() const noexcept { return interval(Axis::horizontal).start; }

        constexpr auto y() const noexcept { return interval(Axis::vertical).start; }

        constexpr auto w() const noexcept { return length(interval(Axis::horizontal)); }

        constexpr auto h() const noexcept { return length(interval(Axis::vertical)); }

        constexpr auto area() const noexcept { return w()*h(); }

    private:

        Interval intervals[int(Axis::size)];
    };

    constexpr auto operator==(Rectangle const lhs, Rectangle const rhs) noexcept
    {
        return lhs.interval(Axis::horizontal)==rhs.interval(Axis::horizontal)
                && lhs.interval(Axis::vertical)==rhs.interval(Axis::vertical);
    }

    constexpr auto operator<(Rectangle const lhs, Rectangle const rhs) noexcept
    {
        return (lhs.interval(Axis::horizontal)<rhs.interval(Axis::horizontal))
                || (lhs.interval(Axis::horizontal)==rhs.interval(Axis::horizontal)
                        && lhs.interval(Axis::vertical)<rhs.interval(Axis::vertical));
    }

    constexpr Rectangle operator&(Rectangle const lhs, Rectangle const rhs) noexcept
    {
        return Rectangle::from_intervals(
                lhs.interval(Axis::horizontal) & rhs.interval(Axis::horizontal),
                lhs.interval(Axis::vertical) & rhs.interval(Axis::vertical));
    }

    constexpr bool is_positive(Rectangle const r) noexcept
    {
        return r.w()>0 && r.h()>0;
    }

    constexpr bool contains(Rectangle const r, int x, int y) noexcept
    {
        return contains(r.interval(Axis::horizontal), x) && contains(r.interval(Axis::vertical), y);
    }

    constexpr Rectangle maximum_rectangle = Rectangle::from_intervals(
            Interval{std::numeric_limits<int>::min(), std::numeric_limits<int>::max()},
            Interval{std::numeric_limits<int>::min(), std::numeric_limits<int>::max()});
}

namespace std {
    template<>
    struct hash<intersections::Rectangle> {
        constexpr std::size_t operator()(intersections::Rectangle const& rectangle) const noexcept
        {
            return static_cast<std::size_t>(rectangle.interval(intersections::Axis::horizontal).start)
                    ^ (static_cast<std::size_t>(rectangle.interval(intersections::Axis::horizontal).end) << 10)
                    ^ (static_cast<std::size_t>(rectangle.interval(intersections::Axis::vertical).start) << 20)
                    ^ (static_cast<std::size_t>(rectangle.interval(intersections::Axis::vertical).end) << 30);
        }
    };
}

#endif //INTERSECTIONS_RECTANGLE_H
