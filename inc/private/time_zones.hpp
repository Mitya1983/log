#ifndef TIME_ZONES_HPP
#define TIME_ZONES_HPP

#include <cstdint>

namespace tristan {

    /**
     * \brief Enum which represents possible time zone offsets.
     */
    enum class TimeZone : int8_t {
        WEST_12 [[maybe_unused]] = -12,
        WEST_11 [[maybe_unused]] = -11,
        WEST_10 [[maybe_unused]] = -10,
        WEST_9 [[maybe_unused]] = -9,
        WEST_8 [[maybe_unused]] = -8,
        WEST_7 [[maybe_unused]] = -7,
        WEST_6 [[maybe_unused]] = -6,
        WEST_5 [[maybe_unused]] = -5,
        WEST_4 [[maybe_unused]] = -4,
        WEST_3 [[maybe_unused]] = -3,
        WEST_2 [[maybe_unused]] = -2,
        WEST_1 [[maybe_unused]] = -1,
        UTC = 0,
        EAST_1 [[maybe_unused]] = 1,
        EAST_2 [[maybe_unused]] = 2,
        EAST_3 [[maybe_unused]] = 3,
        EAST_4 [[maybe_unused]] = 4,
        EAST_5 [[maybe_unused]] = 5,
        EAST_6 [[maybe_unused]] = 6,
        EAST_7 [[maybe_unused]] = 7,
        EAST_8 [[maybe_unused]] = 8,
        EAST_9 [[maybe_unused]] = 9,
        EAST_10 [[maybe_unused]] = 10,
        EAST_11 [[maybe_unused]] = 11,
        EAST_12 [[maybe_unused]] = 12,
    };

}  // namespace tristan

#endif  //TIME_ZONES_HPP
