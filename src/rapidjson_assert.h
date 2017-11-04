//
// Created by john on 10/29/17.
//

/// \file
/// \brief defines RapdJSON error handling macro; ensures basic error handling in release builds

#ifndef INTERSECTIONS_RAPIDJSON_ASSERT_H
#define INTERSECTIONS_RAPIDJSON_ASSERT_H

#if defined(RAPIDJSON_ASSERT)
#error this file should be included before any RapidJSON headers
#endif

#include <cstdio>
#include <cstdlib>

void on_dom_error() noexcept
{
    std::fprintf(stderr, "error in rectangle file format");
    std::abort();
}

#define RAPIDJSON_ASSERT(expr) ((expr) ? ((void)0) : on_dom_error())

#endif //INTERSECTIONS_RAPIDJSON_ASSERT_H
