//
// Created by john on 10/27/17.
//

/// \file
/// \brief definition of intersections::Interval and related functions

#ifndef INTERSECTIONS_INTERVAL_H
#define INTERSECTIONS_INTERVAL_H

#include <algorithm>
#include <cassert>
#include <limits>

namespace intersections {
    // semi-open interval, representing [start, end);
    // used to represent bounds in Rectangle
    struct Interval {
        int start = 0;
        int end = 0;

        constexpr Interval() = default;

        static constexpr Interval from_extent(int start, int extent) noexcept
        {
            // check for out-of-range error
            if (extent>=0) {
                assert(std::numeric_limits<int>::max()-extent>=start);
            }
            else {
                assert(std::numeric_limits<int>::lowest()-extent<=start);
            }
            return Interval{start, start+extent};
        }
    };

    constexpr auto length(Interval const& i) noexcept
    {
        return i.end-i.start;
    }

    constexpr auto operator==(Interval const& lhs, Interval const& rhs) noexcept
    {
        return lhs.start==rhs.start && lhs.end==rhs.end;
    }

    constexpr auto operator!=(Interval const& lhs, Interval const& rhs) noexcept
    {
        return !(lhs==rhs);
    }

    constexpr auto operator<(Interval const lhs, Interval const rhs) noexcept
    {
        return (lhs.start<rhs.start) || (lhs.start==rhs.start && lhs.end<rhs.end);
    }

    constexpr bool contains(Interval const& i, int position) noexcept
    {
        return position>=i.start && position<i.end;
    }

    // returns intersection of a and b
    constexpr auto operator&(Interval const& a, Interval const& b) noexcept
    {
        Interval limited{std::max(a.start, b.start), std::min(a.end, b.end)};
        return Interval{std::min(limited.start, limited.end), limited.end};
    }
}

#endif //INTERSECTIONS_INTERVAL_H
