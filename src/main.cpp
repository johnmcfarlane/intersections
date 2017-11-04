//
// Created by john on 10/17/17.
//

/// \file
/// \brief command-line tool reads JSON file and prints intersections

#include <intersections.h>

#include <memory>

#include "rapidjson_assert.h"
#include <rapidjson/document.h>

namespace {
    auto load_file(char const* const filename)
    {
        auto const file = std::fopen(filename, "rb");
        if (file==nullptr) {
            std::fprintf(stderr, "error opening JSON file, \"%s\"", filename);
            std::exit(EXIT_FAILURE);
        }

        std::fseek(file, 0, SEEK_END);
        auto ftell_result = std::ftell(file);
        if (ftell_result<0) {
            std::fprintf(stderr, "error determining size of JSON file, \"%s\"", filename);
            std::exit(EXIT_FAILURE);
        }
        auto file_size = static_cast<std::size_t>(ftell_result);

        auto buffer = std::unique_ptr<char[]>(new char[file_size+1]);
        if (!buffer) {
            std::fprintf(stderr, "error accomodating JSON file, \"%s\"", filename);
            std::exit(EXIT_FAILURE);
        }

        std::fseek(file, 0, SEEK_SET);
        auto read = std::fread(buffer.get(), file_size, 1, file);
        if (read!=1) {
            std::fprintf(stderr, "error reading JSON file, \"%s\"", filename);
            std::exit(EXIT_FAILURE);
        }
        buffer[file_size] = '\0';

        return buffer;
    }

    auto read_rectangles(rapidjson::Document const& document)
    {
        auto rectangles = intersections::Rectangles{};
        auto const& json_rects = document["rects"].GetArray();
        std::transform(std::begin(json_rects), std::end(json_rects), std::back_inserter(rectangles),
                [](auto const& json_rect) {
                    return intersections::Rectangle {
                            json_rect["x"].GetInt(),
                            json_rect["y"].GetInt(),
                            json_rect["w"].GetInt(),
                            json_rect["h"].GetInt()
                    };
                }
        );
        return rectangles;
    }

    auto print_input(intersections::Rectangles const& rectangles) noexcept
    {
        std::puts("Inputs:");
        auto n = 1;
        for (auto const& rectangle : rectangles) {
            std::printf(
                    "\t%d: Rectangle at (%d,%d), w=%d, h=%d.\n",
                    n++,
                    rectangle.x(), rectangle.y(), rectangle.w(), rectangle.h());
        }
    }

    auto print_solution(intersections::Intersections const& intersections,
            intersections::Rectangle const* rectangles_begin) noexcept
    {
        std::puts("Intersections:");
        for (auto const& intersection : intersections) {
            auto const& intersectees = intersection.second;
            std::printf("\tBetween rectangle ");
            auto print_index = [&](auto const rectangle) {
                std::printf("%d", int(std::distance(rectangles_begin, rectangle)+1));
            };
            assert (!intersectees.empty());
            auto second_from_back = std::prev(std::end(intersectees), 2);
            std::for_each(std::begin(intersectees), second_from_back, [&](auto const rectangle) {
                print_index(rectangle);
                std::printf(", ");
            });
            print_index(*second_from_back);
            std::printf(" and ");
            print_index(*std::prev(std::end(intersectees), 1));

            auto const& overlap = intersection.first;
            std::printf(" at (%d, %d), w=%d, h=%d\n", overlap.x(), overlap.y(), overlap.w(), overlap.h());
        }
    }
}

int main(int argc, char** argv)
{
    // load file into buffer
    if (argc!=2) {
        std::puts("Please provide a rectangles file.");
        return EXIT_FAILURE;
    }
    auto const filename = argv[1];
    auto const buffer = load_file(filename);

    // parse buffer into document
    rapidjson::Document document;
    if (document.ParseInsitu(buffer.get()).HasParseError()) {
        std::fprintf(stderr, "parse error at position %zd of JSON file, \"%s\"", document.GetErrorOffset(),
                filename);
        return EXIT_FAILURE;
    }

    // read rectangles from document
    auto rectangles = read_rectangles(document);

    // print the input list
    print_input(rectangles);
    std::putchar('\n');

    // solve
    auto intersections = intersections::solve<intersections::Solution::fast>(rectangles);

    // print the solutions
    print_solution(intersections, rectangles.data());

    return EXIT_SUCCESS;
}
