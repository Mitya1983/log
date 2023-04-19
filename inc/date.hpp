#ifndef DATE_HPP
#define DATE_HPP

#include "time_zones.hpp"

#include <chrono>
#include <string>
#include <ostream>
#include <functional>

/**
 * \brief Namespace which includes date handlers
 */
namespace tristan::date {

    class Date;

    using Formatter = std::function<std::string(const Date&)>;

    /**
     * \brief Days duration
     */
    using Days = std::chrono::duration< int64_t, std::ratio_divide< std::ratio< 86400 >, std::chrono::seconds::period > >;

    /**
     * \brief Class to handle date
     * \headerfile date.hpp
     */
    class Date {

    public:
        /**
         * \brief Default constructor.
         * Creates Date object which represent current date based on UTC time zone
         */
        Date();
        /**
         * \overload
         * \brief Overloaded constructor
         * Creates Date object which represent current date based on provided time zone
         * \param p_time_zone
         */
        explicit Date(tristan::TimeZone p_time_zone);
        /**
         * \overload
         * \brief Overloaded constructor
         * Creates Date object with specified day, month and year.
         * \param day uint8_t.
         * \param month uint8_t.
         * \param year uint16_t.
         * \throws std::range_error.
         */
        explicit Date(uint8_t day, uint8_t month, uint16_t year);
        /**
         * \overload
         * \brief Overloaded constructor
         * Creates Date object from std::string representing the date in [YYYYMMDD] or [YYYY-MM-DD] formats.
         * \param iso_date const std::string&.
         * \throws std::invalid_argument - if time representation has invalid format.
         * \throws std::range_error - if date representation has invalid values.
         */
        explicit Date(const std::string& iso_date);
        /**
         * \brief Copy constructor
         */
        Date(const Date&) = default;
        /**
         * \brief Move constructor
         */
        Date(Date&&) = default;

        /**
         * \brief Copy assignment operator
         * \return Date&
         */
        auto operator=(const Date&) -> Date& = default;
        /**
         * \brief Move assignment operator
         * \return Date&
         */
        auto operator=(Date&&) -> Date& = default;
        /// \brief Operator equal.
        /**
         * \brief Operator ==
         * \param other const Date&
         * \return bool
         */
        auto operator==(const Date& other) const -> bool;
        /**
         * \brief Operator <
         * \param other const Date&
         * \return bool
         */
        auto operator<(const Date& other) const -> bool;

        /**
         * \brief Destructor
         */
        ~Date() = default;

        /**
         * \brief Adds days
         * \param days uint64_t
         */
        void addDays(uint64_t days);
        /**
         * \brief Adds months
         * \param months uint64_t
         */
        void addMonths(uint64_t months);
        /**
         * \brief Adds years
         * \param years uint64_t
         */
        void addYears(uint64_t years);
        /**
         * \brief Subtracts days
         * \param days uint64_t
         */
        void subtractDays(uint64_t days);
        /**
         * \brief Subtracts months
         * \param months uint64_t
         */
        void subtractMonths(uint64_t months);
        /**
         * \brief Subtracts years
         * \param years uint64_t
         */
        void subtractYears(uint64_t years);
        /**
         * \brief Returns currently set day of the month.
         * \note This function returns actual, or otherworldly current, day of the months and not the total number of days passed in the month.
         * \return uint8_t.
         */
        [[nodiscard]] auto dayOfTheMonth() const -> uint8_t;
        /**
         * \brief Returns currently set day of the week.
         * \note This function returns actual, or otherworldly current, day of the week and not the total number of days passed in the week.
         * \return uint8_t.
         */
        [[nodiscard]] auto dayOfTheWeek() const -> uint8_t;
        /**
         * \brief Returns currently set month of the year.
         * \return Months.
         */
        [[nodiscard]] auto month() const -> uint8_t;
        /**
         * \brief Returns currently set year.
         * \return uint8_t.
         */
        [[nodiscard]] auto year() const -> uint16_t;
        /**
         * \brief Returns if currently set day of the week is weekend.
         * \note Saturday and Sunday are considered as weekend days.
         * \return bool.
         */
        [[nodiscard]] auto isWeekend() const -> bool;
        /**
         * \brief Checks if year is leap year.
         * \param year uint16_t.
         * \return bool.
         */
        [[nodiscard]] static auto isLeapYear(uint16_t year) -> bool;
        /**
         * \brief Sets formatter for class aka for all instances.
         * \param p_formatter std::function<std::string(const Date&)>
         */
        static void setGlobalFormatter(Formatter&& p_formatter);
        /**
         * \brief Sets formatter for particular object of class Time.
         * Local formatter has higher priority then the global formatter. That is if local formatter is set, the latter will be implemented instead of global one.
         * \param p_formatter std::function<std::string(const Date&)>
         */
        void setLocalFormatter(Formatter&& p_formatter);

        /**
         * \brief Return string representation of date in YYYY-MM-DD format.
         * \return std::string.
         */
        [[nodiscard]] auto toString() const -> std::string;

        /**
         * \brief Creates Date object which represents local date.
         * \return Date.
         */
        [[nodiscard]] static auto localDate() -> Date;

    protected:
    private:
        inline static Formatter m_formatter_global;

        Formatter m_formatter_local;

        Days m_days_since_1900;

        [[nodiscard]] auto _calculateCurrentMonth() const -> uint8_t;
        [[nodiscard]] auto _calculateCurrentYear() const -> uint8_t;
        [[nodiscard]] auto _calculateDayOfTheMonth() const -> uint8_t;
        [[nodiscard]] auto _calculateDaysInYear() const -> uint16_t;
    };

    /**
     * \brief Operator !=
     * \param l const Date &
     * \param r const Date &
     * \return bool
     */
    auto operator!=(const Date& l, const Date& r) -> bool;
    /**
     * \brief Operator >
     * \param l const Date &
     * \param r const Date &
     * \return bool
     */
    auto operator>(const Date& l, const Date& r) -> bool;
    /**
     * \brief Operator <=
     * \param l const Date &
     * \param r const Date &
     * \return bool
     */
    auto operator<=(const Date& l, const Date& r) -> bool;
    /**
     * \brief Operator >=
     * \param l const Date &
     * \param r const Date &
     * \return bool
     */
    auto operator>=(const Date& l, const Date& r) -> bool;
    /**
     * \brief Operator <<
     * \param out std::ostream&
     * \param date const Date&
     * \return std::ostream&
     * \note Method toString() is used here
     */
    auto operator<<(std::ostream& out, const Date& date) -> std::ostream&;

}  //namespace tristan::date
#endif  // DATE_HPP
