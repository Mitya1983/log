#ifndef TIME_HPP
#define TIME_HPP

#include "time_zones.hpp"

#include <string>
#include <iostream>
#include <chrono>
#include <variant>
#include <functional>

/**
 * \brief Namespace which includes time handlers
 */
namespace tristan::time {

    class Time;

    using Formatter = std::function<std::string(const Time&)>;

    /**
     * \brief Enum which represents precisions used in implementation.
     */
    enum class Precision : uint8_t {
        MINUTES,
        SECONDS,
        MILLISECONDS,
        MICROSECONDS,
        NANOSECONDS
    };

    /**
     * \brief Class to handle time
     * \headerfile time.hpp
     */
    class Time {
        friend auto operator+(const Time& l, const Time& r) -> Time;
        friend auto operator-(const Time& l, const Time& r) -> Time;

    public:
        /**
         * \brief Default constructor.
         * Creates time based on UTC time zone
         * \param precision Precision which is set to SECONDS
         */
        explicit Time(Precision precision = Precision::SECONDS);
        /**
         * \brief Default constructor.
         * Creates time based on provided time zone
         * \param precision Precision which is set to SECONDS
         */
        explicit Time(tristan::TimeZone p_time_zone, Precision p_precision = Precision::SECONDS);
        /**
         * \overload
         * \brief Overloaded constructor.
         * Precision is set to Precision::MINUTES and offset is set to UTC
         * \param hours uint8_t
         * \param minutes uint8_t
         * \throws std::range_error
         */
        explicit Time(uint8_t hours, uint8_t minutes);
        /**
         * \overload
         * \brief Overloaded constructor.
         * Precision is set to Precision::SECONDS and offset is set to UTC
         * \param hours uint8_t.
         * \param minutes uint8_t.
         * \param seconds uint8_t.
         * \throws std::range_error.
         */
        explicit Time(uint8_t hours, uint8_t minutes, uint8_t seconds);
        /**
         * \overload
         * \brief Overloaded constructor.
         * Precision is set to Precision::MILLISECONDS and offset is set to UTC
         * \param hours uint8_t.
         * \param minutes uint8_t.
         * \param seconds uint8_t.
         * \param milliseconds uint16_t.
         * \throws std::range_error.
         */
        explicit Time(uint8_t hours, uint8_t minutes, uint8_t seconds, uint16_t milliseconds);
        /**
         * \overload
         * \brief Overloaded constructor.
         * Precision is set to Precision::MICROSECONDS and offset is set to UTC
         * \param hours uint8_t.
         * \param minutes uint8_t.
         * \param seconds uint8_t.
         * \param milliseconds uint16_t.
         * \param microseconds uint16_t.
         * \throws std::range_error.
         */
        explicit Time(uint8_t hours, uint8_t minutes, uint8_t seconds, uint16_t milliseconds, uint16_t microseconds);
        /**
         * \overload
         * \brief Overloaded constructor.
         * Precision is set to Precision::NANOSECONDS and offset is set to UTC
         * \param hours uint8_t.
         * \param minutes uint8_t.
         * \param seconds uint8_t.
         * \param milliseconds uint16_t.
         * \param microseconds uint16_t.
         * \param nanoseconds uint16_t.
         * \throws std::range_error.
         */
        explicit Time(uint8_t hours, uint8_t minutes, uint8_t seconds, uint16_t milliseconds, uint16_t microseconds, uint16_t nanoseconds);
        /**
         * \brief Parses the string provided and create time object.
         * \param time std::string representing time in following <b>formats</b>:
         * \li [HH:MM] - Minutes precision.
         * \li [HH:MM+(-)HH] - Minutes precision with offset.
         * \li [HH:MM:SS] - Seconds precision.
         * \li [HH:MM:SS+(-)HH] - Seconds precision with offset.
         * \li [HH:MM:SS.mmm] - Milliseconds precision.
         * \li [HH:MM:SS.mmm+(-)HH] - Milliseconds precision with offset.
         * \li [HH:MM:SS.mmm.mmm] - Microseconds precision.
         * \li [HH:MM:SS.mmm.mmm+(-)HH] - Microseconds precision with offset.
         * \li [HH:MM:SS.mmm.mmm.nnn] - Nanoseconds precision.
         * \li [HH:MM:SS.mmm.mmm.nnn+(-)HH] - Nanoseconds precision with offset.
         * \throws std::invali_argument, std::range_error.
         */
        explicit Time(const std::string& time);
        /**
         * \brief Copy constructor
         */
        Time(const Time&) = default;
        /**
         * \brief Move constructor
         */
        Time(Time&&) = default;
        /**
         * \brief Copy assignment operator
         * \return Time&
         */
        auto operator=(const Time&) -> Time& = default;
        /**
         * \brief Move assignment operator
         * \return Time&
         */
        auto operator=(Time&&) -> Time& = default;
        /**
         * \brief Operator ==
         * \param other const Time&
         * \return bool
         * \note Precision is taken into account. That is if comparable objects are having different precision - false is returned
         */
        auto operator==(const Time& other) const -> bool;
        /**
         * \brief Operator <
         * \param other const Time&
         * \return bool
         * \note Precision is taken into account. That is if comparable objects are having different precision - false is returned.
         */
        auto operator<(const Time& other) const -> bool;
        /**
         * \brief Operator +=
         * \param other const Time&
         */
        void operator+=(const Time& other);
        /**
         * \brief Operator -=
         * \param other const Time&
         */
        void operator-=(const Time& other);
        /**
         * \brief Destructor
         */
        ~Time() = default;

        /**
         * \brief Sets timezone offset. Only ISO hour based offsets are considered.
         * \param offset TimeZone
         */
        [[maybe_unused]] void setOffset(TimeZone offset) { m_offset = offset; }

        /**
         * \brief Adds hours. This is a convenience function which converts hours to minutes and invokes addMinutes(uint64_t minutes) function.
         * \param hours uint64_t
         */
        void addHours(uint64_t hours);
        /**
         * \brief Adds minutes.
         * \param minutes uint64_t
         */
        void addMinutes(uint64_t minutes);
        /**
         * \brief Adds seconds to Time object.
         * \note Precision is taken into account. That is in case of MINUTES precision if number of seconds is less then 1 minute operation is meaningless and will not have any effect.
         * \param seconds uint64_t.
         */
        void addSeconds(uint64_t seconds);
        /**
         * \brief Adds milliseconds to Time object.
         * \note Precision is taken into account. That is in case of SECONDS or MINUTES precision if number of milliseconds is less then 1 second operation is meaningless and will not have any effect.
         * \param milliseconds uint64_t.
         */
        void addMilliseconds(uint64_t milliseconds);
        /**
         * \brief Adds microseconds to Time object.
         * \note Precision is taken into account. That is in case of SECONDS, MINUTES or MILLISECONDS precision if number of microseconds is less then 1 millisecond operation is meaningless and will not have any effect.
         * \param microseconds uint64_t.
         */
        void addMicroseconds(uint64_t microseconds);
        /**
         * \brief Adds nanoseconds to Time object.
         * \note Precision is taken into account. That is in case of SECONDS, MINUTES, MILLISECONDS or MICROSECONDS precision if number of nanosecond is less then 1 microsecond operation is meaningless and will not have any effect.
         * \param nanoseconds uint64_t.
         */
        void addNanoseconds(uint64_t nanoseconds);
        /**
         * \brief subtracts hours from Time object. This is a convenience function which converts hours to minutes and invokes subtractMinutes(uint64_t minutes) function
         * \param hours uint64_t.
         */
        void subtractHours(uint64_t hours);
        /**
         * \brief subtracts minutes from Time object.
         * \param minutes uint64_t.
         */
        void subtractMinutes(uint64_t minutes);
        /**
         * \brief subtracts seconds from Time object.
         * \note Precision is taken into account. That is in case of MINUTES precision if number of seconds is less then 1 minute operation is meaningless and will not have any effect.
         * \param seconds uint64_t.
         */
        void subtractSeconds(uint64_t seconds);
        /**
         * \brief subtracts milliseconds from Time object.
         * \note Precision is taken into account. That is in case of SECONDS or MINUTES precision if number of milliseconds is less then 1 second operation is meaningless and will not have any effect.
         * \param milliseconds uint64_t.
         */
        void subtractMilliseconds(uint64_t milliseconds);
        /**
         * \brief subtracts microseconds from Time object.
         * \note Precision is taken into account. That is in case of SECONDS, MINUTES, MILLISECONDS or MICROSECONDS precision if number of nanosecond is less then 1 microsecond operation is meaningless and will not have any effect.
         * \param microseconds uint64_t.
         */
        void subtractMicroseconds(uint64_t microseconds);
        /**
         * \brief Subtracts nanoseconds from Time object.
         * \note Precision is taken into account. That is in case of SECONDS, MINUTES, MILLISECONDS or MICROSECONDS precision if number of nanosecond is less then 1 microsecond operation is meaningless and will not have any effect
         * \param nanoseconds uint64_t
         */
        void subtractNanoseconds(uint64_t nanoseconds);

        /**
         * \brief Returns number of hours passed since day start.
         * \return uint8_t
         */
        [[nodiscard]] auto hours() const -> uint8_t;
        /**
         * \brief Returns number of minutes passed since hour start.
         * \return uint8_t
         */
        [[nodiscard]] auto minutes() const -> uint8_t;
        /**
         * \brief Returns
         * \return uint8_t
         */
        [[nodiscard]] auto seconds() const -> uint8_t;
        /**
         * \brief Returns number of milliseconds passed since second start.
         * \return uint16_t
         */
        [[nodiscard]] auto milliseconds() const -> uint16_t;
        /**
         * \brief Returns number of microseconds passed since millisecond start.
         * \return uint16_t
         */
        [[nodiscard]] auto microseconds() const -> uint16_t;
        /**
         * \brief Returns number of nanoseconds passed since microsecond start.
         * \return uint16_t
         */
        [[nodiscard]] auto nanoseconds() const -> uint16_t;
        /**
         * \brief Returns precision of Time object.
         * \return Precision
         */
        [[nodiscard]] auto precision() const -> Precision { return m_precision; }

        /**
         * \brief Returns current offset
         * \return
         */
        [[nodiscard]] auto offset() const -> TimeZone { return m_offset; }
        /**
         * \brief Creates Time object which represents localtime.
         * \param precision Precision::SECONDS.
         * \return Time.
         */
        [[nodiscard]] static auto localTime(Precision precision = Precision::SECONDS) -> Time;

        /**
         * \brief Sets formatter for class aka for all instances.
         * \param p_formatter std::function<std::string(const Time&)>
         */
        static void setGlobalFormatter(Formatter&& p_formatter);

        /**
         * \brief Sets formatter for particular object of class Time.
         * Local formatter has higher priority then the global formatter. That is if local formatter is set, the latter will be implemented instead of global one.
         * \param p_formatter std::function<std::string(const Time&)>
         */
        void setLocalFormatter(Formatter&& p_formatter);

        /**
         * \brief Generates default string representation of time which is ISO standard representation in formats represented below.
         * \return std::string.
         * \par Formats:
         * \li [hours:minutes] - Minutes precision.
         * \li [hours:minutes:seconds] - Seconds precision.
         * \li [hours:minutes:seconds.milliseconds] - Milliseconds precision.
         * \li [hours:minutes:seconds.milliseconds.microseconds] - Microseconds precision.
         * \li [hours:minutes:seconds.milliseconds.microseconds.nanoseconds] - Nanoseconds precision.
         */
        [[nodiscard]] auto toString() const -> std::string;


    protected:
    private:
        inline static Formatter m_formatter_global;

        Formatter m_formatter_local;

        std::variant< std::chrono::minutes, std::chrono::seconds, std::chrono::milliseconds, std::chrono::microseconds, std::chrono::nanoseconds >
            m_time_since_day_start;

        TimeZone m_offset;

        Precision m_precision;
        void _addMinutes(uint64_t minutes);
        void _addSeconds(uint64_t seconds);
        void _addMilliseconds(uint64_t milliseconds);
        void _addMicroseconds(uint64_t microseconds);
        void _addNanoseconds(uint64_t nanoseconds);
        void _subtractMinutes(uint64_t minutes);
        void _subtractSeconds(uint64_t seconds);
        void _subtractMilliseconds(uint64_t milliseconds);
        void _subtractMicroseconds(uint64_t microseconds);
        void _subtractNanoseconds(uint64_t nanoseconds);
    };

    /**
     * \brief Operator !=
     * \param l const Time&
     * \param r const Time&
     * \return bool
     */
    auto operator!=(const Time& l, const Time& r) -> bool;
    /**
     * \brief Operator >
     * \param l const Time&
     * \param r const Time&
     * \return bool
     */
    auto operator>(const Time& l, const Time& r) -> bool;
    /**
     * \brief Operator <=
     * \param l const Time&
     * \param r const Time&
     * \return bool
     */
    auto operator<=(const Time& l, const Time& r) -> bool;
    /**
     * \brief Operator >=
     * \param l const Time&
     * \param r const Time&
     * \return bool
     */
    auto operator>=(const Time& l, const Time& r) -> bool;

    /**
     * \brief Operator +
     * \param l const Time&
     * \param r const Time&
     * \return Time
     */
    auto operator+(const Time& l, const Time& r) -> Time;
    /**
     * \brief Operator -
     * \param l const Time&
     * \param r const Time&
     * \return Time
     */
    auto operator-(const Time& l, const Time& r) -> Time;
    /**
     * \brief Operator <<
     * \param out std::ostream&
     * \param time const Time&
     * \return std::ostream&
     * \note Method toString() is used here
     */
    auto operator<<(std::ostream& out, const Time& time) -> std::ostream&;
}  //namespace tristan::time

#endif  // TIME_HPP
