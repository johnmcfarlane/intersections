//
// Created by john on 10/19/17.
//

/// \file
/// \brief basic tests of the functionality provided via the intersections::solve API

#include <intersections.h>

#include <chrono>
#include <random>
#include <unordered_set>

using intersections::RectangleSequence;
using intersections::Intersections;
using intersections::Interval;
using intersections::Rectangle;
using intersections::Rectangles;
using intersections::Solution;
using intersections::solve;

// simple assert macro designed for facilitating tests
#define TEST_ASSERT(CONDITION) \
do { if (!(CONDITION)) {\
        std::fprintf(stderr, "%s:%d: test failed \"%s\"\n", __FILE__, __LINE__, #CONDITION); \
        std::abort(); \
}} while(false)

namespace {
    ////////////////////////////////////////////////////////////////////////////////
    // hand-crafted unit tests

    template<Solution solution>
    void test_empty()
    {
        auto rectangles = Rectangles{};
        auto expected = Intersections{};
        auto actual = solve<solution>(rectangles);
        TEST_ASSERT(expected==actual);
    }

    template<Solution solution>
    void test_solitary()
    {
        auto rectangles = Rectangles{Rectangle {1, 1, 1, 1}};
        auto expected = Intersections{};
        auto actual = solve<solution>(rectangles);
        TEST_ASSERT(expected==actual);
    }

    template<Solution solution>
    void test_two_not_overlapping()
    {
        auto rectangles = Rectangles{
                Rectangle {0, 0, 10, 10},
                Rectangle {15, 15, 10, 10}};
        auto expected = Intersections{};
        auto actual = solve<solution>(rectangles);
        TEST_ASSERT(expected==actual);
    }

    template<Solution solution>
    void test_two_touching()
    {
        auto rectangles = Rectangles{
                Rectangle {0, 0, 10, 10},
                Rectangle {10, 10, 10, 10}};
        auto expected = Intersections{};
        auto actual = solve<solution>(rectangles);
        TEST_ASSERT(expected==actual);
    }

    template<Solution solution>
    void test_two_overlapping()
    {
        auto rectangles = Rectangles{
                Rectangle {0, 0, 10, 10},
                Rectangle {5, 5, 10, 10}};
        auto expected = Intersections{{
                                              Intersections::value_type{Rectangle {5, 5, 5, 5},
                                                                        RectangleSequence {&rectangles[0],
                                                                                           &rectangles[1]}}
                                      }};
        auto actual = solve<solution>(rectangles);
        TEST_ASSERT(expected==actual);
    }

    template<Solution solution>
    void test_two_concentric()
    {
        auto rectangles = Rectangles{
                Rectangle {1, 2, 17, 30},
                Rectangle {5, 3, 10, 10}};
        auto expected = Intersections{{
                                              Intersections::value_type{Rectangle {5, 3, 10, 10},
                                                                        RectangleSequence {&rectangles[0],
                                                                                           &rectangles[1]}}

                                      }};
        auto actual = solve<solution>(rectangles);
        TEST_ASSERT(expected==actual);
    }

    template<Solution solution>
    void test_three_overlapping_chain_loose()
    {
        auto rectangles = Rectangles{
                Rectangle {0, 0, 10, 10},
                Rectangle {6, 6, 10, 10},
                Rectangle {12, 12, 10, 10}};
        auto expected = Intersections{{
                                              Intersections::value_type{
                                                      Rectangle {6, 6, 4, 4},
                                                      RectangleSequence {&rectangles[0], &rectangles[1]}},
                                              Intersections::value_type{
                                                      Rectangle {12, 12, 4, 4},
                                                      RectangleSequence {&rectangles[1], &rectangles[2]}}}};
        auto actual = solve<solution>(rectangles);
        TEST_ASSERT(expected==actual);
    }

    template<Solution solution>
    void test_three_overlapping_chain_tight()
    {
        // +--------+
        // |        |
        // | 1      |
        // |  +-----+--+
        // |  |     |  |
        // |  | 2   |1 |
        // |  |  +--+--+--+
        // |  |  |  |  |  |
        // |  |  | 3|  |  |
        // +--+--+--+  |  |
        //    | 1|   2 |  |
        //    |  |     |  |
        //    +--+-----+  |
        //       |      1 |
        //       |        |
        //       +--------+
        auto rectangles = Rectangles{
                Rectangle {0, 0, 10, 10},
                Rectangle {3, 3, 10, 10},
                Rectangle {6, 6, 10, 10}};
        auto expected = Intersections
                {{
                         Intersections::value_type{
                                 Rectangle {3, 3, 7, 7},
                                 RectangleSequence {&rectangles[0], &rectangles[1]}},
                         Intersections::value_type{
                                 Rectangle {6, 6, 4, 4},
                                 RectangleSequence {&rectangles[0], &rectangles[1], &rectangles[2]}},
                         Intersections::value_type{
                                 Rectangle {6, 6, 7, 7},
                                 RectangleSequence {&rectangles[1], &rectangles[2]}}
                 }};
        auto actual = solve<solution>(rectangles);
        TEST_ASSERT(expected==actual);
    }

    template<Solution solution>
    void test_three_overlapping_completely()
    {
        auto rectangles = Rectangles{
                Rectangle {0, 0, 1, 1},
                Rectangle {0, 0, 1, 1},
                Rectangle {0, 0, 1, 1}};
        auto expected = Intersections
                {{
                         Intersections::value_type{
                                 Rectangle {0, 0, 1, 1},
                                 RectangleSequence {&rectangles[0], &rectangles[1], &rectangles[2]}}
                 }};
        auto actual = solve<solution>(rectangles);
        TEST_ASSERT(expected==actual);
    }

    template<Solution solution>
    void test_three_regression1()
    {
        // +--+--+--+
        // |..|#.|..|
        // |##|#.|#.|
        // +--+--+--+
        auto rectangles = Rectangles{
                Rectangle {-1, 0, 2, 1},
                Rectangle {-1, -1, 1, 2},
                Rectangle {-1, 0, 1, 1}};

        // +--+
        // |..|
        // |#.|
        // +--+
        auto expected = Intersections
                {{
                         Intersections::value_type{
                                 Rectangle {-1, 0, 1, 1},
                                 RectangleSequence {&rectangles[0], &rectangles[1], &rectangles[2]}}
                 }};
        auto actual = solve<solution>(rectangles);
        TEST_ASSERT(expected==actual);
    }

    template<Solution solution>
    void test_three_regression2()
    {
        auto rectangles = Rectangles{
                Rectangle {0, 0, 1, 1},
                Rectangle {-1, -1, 2, 2},
                Rectangle {0, -1, 1, 2}};
        auto expected = Intersections
                {{
                         Intersections::value_type{
                                 Rectangle {0, 0, 1, 1},
                                 RectangleSequence {&rectangles[0], &rectangles[1], &rectangles[2]}},
                         Intersections::value_type{
                                 Rectangle {0, -1, 1, 2},
                                 RectangleSequence {&rectangles[1], &rectangles[2]}}
                 }};
        auto actual = solve<solution>(rectangles);
        TEST_ASSERT(expected==actual);
    }

    template<Solution solution>
    void test_three_regression3()
    {
        // +----+----+----+
        // +####+    +    +
        // +####+ ###+    +
        // +    + ###+    +
        // +    +    + ###+
        // +----+----+----+
        auto rectangles = Rectangles{
                Rectangle {0, 0, 4, 2},
                Rectangle {1, 1, 3, 2},
                Rectangle {1, 3, 3, 1}};

        auto expected = Intersections
                {{
                         Intersections::value_type{
                                 Rectangle {1, 1, 3, 1},
                                 RectangleSequence {&rectangles[0], &rectangles[1]}}
                 }};
        auto actual = solve<solution>(rectangles);
        TEST_ASSERT(expected==actual);
    }

    template<Solution solution>
    void test_three_regression4()
    {
        // +---+---+---+
        // + ##+   +## +
        // + ##+ ##+## +
        // + ##+ ##+## +
        // +---+---+---+
        auto rectangles = Rectangles{
                Rectangle{1, 0, 2, 3},
                Rectangle{1, 1, 2, 2},
                Rectangle{0, 0, 2, 3}
        };
        auto expected = Intersections
                {{
                         Intersections::value_type{
                                 Rectangle {1, 0, 1, 3},
                                 RectangleSequence {&rectangles[0], &rectangles[2]}},
                         Intersections::value_type{
                                 Rectangle {1, 1, 1, 2},
                                 RectangleSequence {&rectangles[0], &rectangles[1], &rectangles[2]}},
                         Intersections::value_type{
                                 Rectangle {1, 1, 2, 2},
                                 RectangleSequence {&rectangles[0], &rectangles[1]}}
                 }};
        auto actual = solve<solution>(rectangles);
        TEST_ASSERT(expected==actual);
    }

    template<Solution solution>
    void test_four_in_tartan()
    {
        auto rectangles = Rectangles{
                Rectangle {0, 0, 1, 2},
                Rectangle {1, 0, 1, 2},
                Rectangle {0, 0, 2, 1},
                Rectangle {0, 1, 2, 1}};
        auto expected = Intersections{{
                                              Intersections::value_type{Rectangle {0, 0, 1, 1},
                                                                        RectangleSequence {&rectangles[0],
                                                                                           &rectangles[2]}},
                                              Intersections::value_type{Rectangle {1, 0, 1, 1},
                                                                        RectangleSequence {&rectangles[1],
                                                                                           &rectangles[2]}},
                                              Intersections::value_type{Rectangle {0, 1, 1, 1},
                                                                        RectangleSequence {&rectangles[0],
                                                                                           &rectangles[3]}},
                                              Intersections::value_type{Rectangle {1, 1, 1, 1},
                                                                        RectangleSequence {&rectangles[1],
                                                                                           &rectangles[3]}}
                                      }};
        auto actual = solve<solution>(rectangles);
        TEST_ASSERT(expected==actual);
    }

    template<Solution solution>
    void test_four_on_left()
    {
        // +--+--+--+--+
        // |##|#.|#.|..|
        // |##|#.|..|#.|
        // +--+--+--+--+
        auto rectangles = Rectangles{
                Rectangle {0, 0, 2, 2},
                Rectangle {0, 0, 1, 2},
                Rectangle {0, 0, 1, 1},
                Rectangle {0, 1, 1, 1}};

        // +--+--+--+
        // |#.|#.|..|
        // |#.|..|#.|
        // +--+--+--+
        auto expected = Intersections{{
                                              Intersections::value_type{Rectangle {0, 0, 1, 2},
                                                                        RectangleSequence {&rectangles[0],
                                                                                           &rectangles[1]}},
                                              Intersections::value_type{
                                                      Rectangle {0, 0, 1, 1},
                                                      RectangleSequence {&rectangles[0], &rectangles[1],
                                                                         &rectangles[2]}},
                                              Intersections::value_type{
                                                      Rectangle {0, 1, 1, 1},
                                                      RectangleSequence {&rectangles[0], &rectangles[1],
                                                                         &rectangles[3]}}
                                      }};
        auto actual = solve<solution>(rectangles);
        TEST_ASSERT(expected==actual);
    }

    template<Solution solution>
    void test_four_overlapping_various()
    {
        auto rectangles = Rectangles{
                Rectangle {0, 0, 2, 2},
                Rectangle {0, 0, 3, 2},
                Rectangle {0, 0, 3, 1},
                Rectangle {1, 0, 2, 2}};
        auto expected = Intersections
                {{
                         Intersections::value_type{
                                 Rectangle {0, 0, 2, 2},
                                 RectangleSequence {&rectangles[0], &rectangles[1]}},
                         Intersections::value_type{
                                 Rectangle {0, 0, 2, 1},
                                 RectangleSequence {&rectangles[0], &rectangles[1], &rectangles[2]}},
                         Intersections::value_type{
                                 Rectangle {1, 0, 1, 1},
                                 RectangleSequence {&rectangles[0], &rectangles[1], &rectangles[2],
                                                    &rectangles[3]}},
                         Intersections::value_type{
                                 Rectangle {1, 0, 2, 1},
                                 RectangleSequence {&rectangles[1], &rectangles[2], &rectangles[3]}},
                         Intersections::value_type{
                                 Rectangle {1, 0, 2, 2},
                                 RectangleSequence {&rectangles[1], &rectangles[3]}},
                         Intersections::value_type{
                                 Rectangle {0, 0, 3, 1},
                                 RectangleSequence {&rectangles[1], &rectangles[2]}},
                         Intersections::value_type{
                                 Rectangle {1, 0, 1, 2},
                                 RectangleSequence {&rectangles[0], &rectangles[1], &rectangles[3]}},
                 }};
        auto actual = solve<solution>(rectangles);
        TEST_ASSERT(expected==actual);
    }

    template<Solution solution>
    void test_four_regression2()
    {
        auto rectangles = Rectangles{
                Rectangle{1, 1, 1, 1},
                Rectangle{0, 1, 2, 1},
                Rectangle{0, 0, 2, 2},
                Rectangle{0, 0, 2, 1}
        };
        auto expected = Intersections
                {{
                         Intersections::value_type{
                                 Rectangle {1, 1, 1, 1},
                                 RectangleSequence {&rectangles[0], &rectangles[1], &rectangles[2]}},
                         Intersections::value_type{
                                 Rectangle {0, 0, 2, 1},
                                 RectangleSequence {&rectangles[2], &rectangles[3]}},
                         Intersections::value_type{
                                 Rectangle {0, 1, 2, 1},
                                 RectangleSequence {&rectangles[1], &rectangles[2]}}
                 }};
        auto actual = solve<solution>(rectangles);
        TEST_ASSERT(expected==actual);
    }

    template<Solution solution>
    void test_four_regression1()
    {
        // +---+---+---+---+
        // +###+ ##+   +## +
        // +###+ ##+ ##+## +
        // +   + ##+ ##+## +
        // +---+---+---+---+
        auto rectangles = Rectangles{
                Rectangle{0, 0, 3, 2},
                Rectangle{1, 0, 2, 3},
                Rectangle{1, 1, 2, 2},
                Rectangle{0, 0, 2, 3}
        };
        auto expected = Intersections
                {{
                         Intersections::value_type{
                                 Rectangle {1, 0, 2, 2},
                                 RectangleSequence {&rectangles[0], &rectangles[1]}},
                         Intersections::value_type{
                                 Rectangle {1, 1, 2, 2},
                                 RectangleSequence {&rectangles[1], &rectangles[2]}},
                         Intersections::value_type{
                                 Rectangle {1, 1, 2, 1},
                                 RectangleSequence {&rectangles[0], &rectangles[1], &rectangles[2]}},
                         Intersections::value_type{
                                 Rectangle {0, 0, 2, 2},
                                 RectangleSequence {&rectangles[0], &rectangles[3]}},
                         Intersections::value_type{
                                 Rectangle {1, 0, 1, 2},
                                 RectangleSequence {&rectangles[0], &rectangles[1], &rectangles[3]}},
                         Intersections::value_type{
                                 Rectangle {1, 0, 1, 3},
                                 RectangleSequence {&rectangles[1], &rectangles[3]}},
                         Intersections::value_type{
                                 Rectangle {1, 1, 1, 2},
                                 RectangleSequence {&rectangles[1], &rectangles[2], &rectangles[3]}},
                         Intersections::value_type{
                                 Rectangle {1, 1, 1, 1},
                                 RectangleSequence {&rectangles[0], &rectangles[1], &rectangles[2], &rectangles[3]}}
                 }};
        auto actual = solve<solution>(rectangles);
        TEST_ASSERT(expected==actual);
    }

    template<Solution solution>
    void test_example()
    {
        auto rectangles = Rectangles{
                Rectangle {100, 100, 250, 80},
                Rectangle {120, 200, 250, 150},
                Rectangle {140, 160, 250, 100},
                Rectangle {160, 140, 350, 190}};
        auto expected = Intersections{{
                                              Intersections::value_type{Rectangle {140, 160, 210, 20},
                                                                        RectangleSequence {&rectangles[0],
                                                                                           &rectangles[2]}},
                                              Intersections::value_type{Rectangle {160, 140, 190, 40},
                                                                        RectangleSequence {&rectangles[0],
                                                                                           &rectangles[3]}},
                                              Intersections::value_type{Rectangle {140, 200, 230, 60},
                                                                        RectangleSequence {&rectangles[1],
                                                                                           &rectangles[2]}},
                                              Intersections::value_type{Rectangle {160, 200, 210, 130},
                                                                        RectangleSequence {&rectangles[1],
                                                                                           &rectangles[3]}},
                                              Intersections::value_type{Rectangle {160, 160, 230, 100},
                                                                        RectangleSequence {&rectangles[2],
                                                                                           &rectangles[3]}},
                                              Intersections::value_type{
                                                      Rectangle {160, 160, 190, 20},
                                                      RectangleSequence {&rectangles[0], &rectangles[2],
                                                                         &rectangles[3]}},
                                              Intersections::value_type{
                                                      Rectangle {160, 200, 210, 60},
                                                      RectangleSequence {&rectangles[1], &rectangles[2],
                                                                         &rectangles[3]}}
                                      }};
        auto actual = solve<solution>(rectangles);
        TEST_ASSERT(expected==actual);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // "heavy" unit test: procedural stress test

    enum class TestMode {
        // perform additional (slow) tests for correctness of solution
                correctness,

        // perform minimal tests; concentrate on speed of solution
                speed
    };

    // parameters governing heavy tests
    struct Parameters {
        // the number of random rectangles to generate
        int num_rectangles{};

        int edge_magnitude{};

        // maximum extent of random rectangle to generate
        Rectangle max_rectangle;

        // number of iterations using the above parameters
        int num_samples{};

        // type of test to perform
        TestMode test_mode{};
    };

    // returns a random rectangle of positive area not exceeding extent of maximum
    template<typename RandomGenerator>
    Rectangle random(RandomGenerator& random_generator, Rectangle maximum)
    {
        TEST_ASSERT(is_positive(maximum));

        using intersections::Axis;
        using intersections::Interval;

        auto random_interval = [&](Axis axis) {
            auto maximum_interval = maximum.interval(axis);
            std::uniform_int_distribution<> random_position(maximum_interval.start, maximum_interval.end-1);
            auto i = Interval{random_position(random_generator), random_position(random_generator)};
            if (i.start>i.end) {
                using std::swap;
                swap(i.start, i.end);
            }
            ++i.end;
            return i;
        };

        return Rectangle::from_intervals(random_interval(Axis::horizontal), random_interval(Axis::vertical));
    }

    template<Solution solution>
    auto test_heavy(Parameters const& params, std::mt19937 gen)
    {
        auto checksum = 0L;

        for (auto sample = 0; sample!=params.num_samples; ++sample) {
            Rectangles rectangles;
            std::generate_n(std::back_inserter(rectangles), params.num_rectangles, [&]() {
                return random(gen, params.max_rectangle);
            });

            auto const actual = solve<solution>(rectangles);
            checksum += actual.size();

            if (params.test_mode==TestMode::correctness) {
                for (auto y = params.max_rectangle.y(), y_end = y+params.max_rectangle.w(); y!=y_end; ++y) {
                    for (auto x = params.max_rectangle.x(), x_end = x+params.max_rectangle.h(); x!=x_end; ++x) {
                        auto constituents = std::unordered_set<Rectangle const*>{};
                        auto overlap = intersections::maximum_rectangle;
                        for (auto const& rectangle : rectangles) {
                            if (!contains(rectangle, x, y)) {
                                continue;
                            }

                            constituents.insert(&rectangle);
                            overlap = overlap & rectangle;
                        }

                        auto found = actual.find(overlap);
                        if (found==actual.end()) {
                            TEST_ASSERT(constituents.size()<2);
                        }
                        else {
                            //TEST_ASSERT(found->second == constituents);
                        }
                    }
                }
            }
        }

        return checksum;
    }

    template<Solution solution>
    auto test_heavy(int num_samples, Interval num_rectangles, Interval edge_magnitude, TestMode test_mode)
    {
        using intersections::Interval;

        auto checksum = 0L;

        Parameters params;
        params.num_samples = num_samples;
        params.test_mode = test_mode;

        std::mt19937 gen;
        for (params.num_rectangles = num_rectangles.start;
             params.num_rectangles<=num_rectangles.end;
             ++params.num_rectangles) {
            for (params.edge_magnitude = edge_magnitude.start;
                 params.edge_magnitude<=edge_magnitude.end;
                 ++params.edge_magnitude) {
                params.max_rectangle = Rectangle::from_intervals(
                        Interval{0, params.edge_magnitude}, Interval{0, params.edge_magnitude});
                checksum += test_heavy<solution>(params, gen);
            }
        }

        return checksum;
    }

    template<Solution solution>
    auto test_heavy_for_speed(int num_samples, Interval num_rectangles, Interval edge_magnitude)
    {
        std::printf("Running speed test... ");
        std::fflush(stdout);
        auto const start_time = std::chrono::steady_clock::now();
        auto speed_checksum = test_heavy<solution>(num_samples, num_rectangles, edge_magnitude, TestMode::speed);
        auto const finish_time = std::chrono::steady_clock::now();
        auto const duration = std::chrono::duration<double>{finish_time-start_time};
        std::printf("%lg seconds, sum=%ld\n", duration.count(), speed_checksum);

        return speed_checksum;
    }

    template<Solution solution>
    auto test_heavy_for_correctness(int num_samples, Interval num_rectangles, Interval edge_magnitude)
    {
        std::printf("Running correctness test... ");
        std::fflush(stdout);
        auto correctness_checksum = test_heavy<solution>(num_samples, num_rectangles, edge_magnitude,
                TestMode::correctness);
        std::puts("passed");

        return correctness_checksum;
    }

    template<Solution solution>
    auto test_heavy(int num_samples, Interval num_rectangles, Interval edge_magnitude)
    {
        auto speed_checksum = test_heavy_for_speed<solution>(num_samples, num_rectangles, edge_magnitude);
        auto correctness_checksum = test_heavy_for_correctness<solution>(num_samples, num_rectangles, edge_magnitude);

        // check they did not diverge wildly
        TEST_ASSERT(speed_checksum==correctness_checksum);
    }

    template<Solution solution>
    void generate_data(int max_rectangles_bits)
    {
        for (auto e = 0; e<=max_rectangles_bits; ++e) {
            for (auto f = 2; f<=3; ++f) {
                auto n = f << e;
                std::printf("%d rectangles ... ", n);
                test_heavy_for_speed<solution>(1, Interval{n, n}, Interval{250, 250});
            }
        }
    }


    ////////////////////////////////////////////////////////////////////////////////
    // complete test suite for a given solution

    template<Solution solution>
    void test_hand_crafted()
    {
        test_empty<solution>();
        test_solitary<solution>();
        test_two_not_overlapping<solution>();
        test_two_touching<solution>();
        test_two_overlapping<solution>();
        test_two_concentric<solution>();
        test_three_overlapping_chain_loose<solution>();
        test_three_overlapping_chain_tight<solution>();
        test_three_overlapping_completely<solution>();
        test_three_regression1<solution>();
        test_three_regression2<solution>();
        test_three_regression3<solution>();
        test_three_regression4<solution>();
        test_four_in_tartan<solution>();
        test_four_on_left<solution>();
        test_four_overlapping_various<solution>();
        test_four_regression1<solution>();
        test_four_regression2<solution>();
        test_example<solution>();
    }
}

int main()
{
    test_hand_crafted<Solution::fast>();
    test_hand_crafted<Solution::simple>();

    puts("\nTesting fast solution:");
    test_heavy<Solution::fast>(1000, Interval{0, 10}, Interval{50, 50});

    puts("\nTesting simple solution:");
    test_heavy<Solution::simple>(1000, Interval{0, 10}, Interval{50, 50});

    puts("\nGenerating simple graph data:");
    generate_data<Solution::simple>(5);

    puts("\nGenerating fast graph data:");
    generate_data<Solution::fast>(9);

    // the previous test never passes due to an out-of-memory condition
    puts("\nCongratulations, you have more RAM than me!");
}
