#ifndef DATE_TIME_HPP
#define DATE_TIME_HPP
#include "date.hpp"
#include "time.hpp"

/**
 * \brief Namespace which unites date and time in one DateTime object
 */
namespace tristan::date_time {

    class DateTime;

    using Formatter = std::function< std::string(const DateTime&) >;

    /**
     * \brief Class to store date and day time
     * \headerfile date_time.hpp
     */
    class DateTime {
    public:
        /**
         * \brief Default constructor.
         * Creates DateTime based on UTC time zone
         * \param p_precision tristan::time::Precision. Default is set to tristan::time::Precision::SECONDS
         */
        explicit DateTime(tristan::time::Precision p_precision = tristan::time::Precision::SECONDS);
        /**
         * \brief Default constructor.
         * Creates DateTime based on provided time zone
         * \param p_precision tristan::time::Precision. Default is set to tristan::time::Precision::SECONDS
         */
        explicit DateTime(tristan::TimeZone p_time_zone, tristan::time::Precision p_precision = tristan::time::Precision::SECONDS);
        /**
         * \brief String constructor.
         * \param date_time Date and time string representation
         * \li [YYYYMMDDTHH:MM:SS]
         * \li [YYYY-MM-DDTHH:MM:SS]
         * \li [YYYYMMDDTHH:MM:SS+(-)HH]
         * \li [YYYY-MM-DDTHH:MM:SS+(-)HH]
         * \li [YYYYMMDDTHH:MM:SS.mmm]
         * \li [YYYY-MM-DDTHH:MM:SS.mmm]
         * \li [YYYYMMDDTHH:MM:SS.mmm+(-)HH]
         * \li [YYYY-MM-DDTHH:MM:SS.mmm+(-)HH]
         * \li [YYYYMMDDTHH:MM:SS.mmm.mmm]
         * \li [YYYY-MM-DDTHH:MM:SS.mmm.mmm]
         * \li [YYYYMMDDTHH:MM:SS.mmm.mmm+(-)HH]
         * \li [YYYY-MM-DDTHH:MM:SS.mmm.mmm+(-)HH]
         * \li [YYYYMMDDTHH:MM:SS.mmm.mmm.nnn]
         * \li [YYYY-MM-DDTHH:MM:SS.mmm.mmm.nnn]
         * \li [YYYYMMDDTHH:MM:SS.mmm.mmm.nnn+(-)HH]
         * \li [YYYY-MM-DDTHH:MM:SS.mmm.mmm.nnn+(-)HH]
         */
        explicit DateTime(const std::string& date_time);
        /**
         * \brief Copy constructor
         */
        DateTime(const DateTime&) = default;
        /**
         * \brief Move constructor
         */
        DateTime(DateTime&&) = default;
        /**
         * \brief Copy assignment operator
         * \return DateTime&
         */
        auto operator=(const DateTime&) -> DateTime& = default;
        /**
         * \brief Move assignment operator
         * \return DateTime&
         */
        auto operator=(DateTime&&) -> DateTime& = default;
        /**
         * \brief Operator ==
         * \param other const DateTime&
         * \return bool
         */
        auto operator==(const DateTime& other) const -> bool;
        /**
         * \brief Operator <
         * \param other const DateTime&
         * \return bool
         */
        auto operator<(const DateTime& other) const -> bool;
        /**
         * \brief Destructor
         */
        ~DateTime() = default;
        /**
         * \brief Copy assignment setter
         * \param date const date::Date&
         */
        void setDate(const date::Date& p_date);
        /**
         * \brief Move assignment setter
         * \param date date::Date&&
         */
        void setDate(date::Date&& p_date);
        /**
         * \brief Copy assignment setter
         * \param p_time const time::Time&
         */
        void setTime(const time::Time& p_time);
        /**
         * \brief Move assignment setter
         * \param p_time time::Time&&
         */
        void setTime(time::Time&& p_time);
        /**
         * \brief Returns date
         * \return const date::Date&
         */
        [[nodiscard]] auto date() const -> const date::Date&;
        /**
         * \brief Returns time
         * \return const time::Time&
         */
        [[nodiscard]] auto time() const -> const time::Time&;

        /**
         * \brief Sets formatter for object of class Time.
         * Local formatter has higher priority then the global formatter. That is if local formatter is set, the latter will be implemented instead of global one.
         * \param p_formatter std::function<std::string(const Time&)>
         */
        void setTimeLocalFormatter(time::Formatter&& p_formatter);
        /**
         * \brief Sets formatter for object of class Date.
         * Local formatter has higher priority then the global formatter. That is if local formatter is set, the latter will be implemented instead of global one.
         * \param p_formatter std::function<std::string(const Date&)>
         */
        void setDateLocalFormatter(date::Formatter&& p_formatter);

        /**
         * \brief Sets formatter for class aka for all instances.
         * \param p_formatter std::function<std::string(const DateTime&)>
         */
        static void setGlobalFormatter(Formatter&& p_formatter);
        /**
         * \brief Sets formatter for particular object of class DateTime.
         * Local formatter has higher priority then the global formatter. That is if local formatter is set, the latter will be implemented instead of global one.
         * \param p_formatter std::function<std::string(const DateTime&)>
         */
        void setLocalFormatter(Formatter&& p_formatter);
        /**
         * \brief Generates string representation of time. Returns [Date::toString][T][Time::toString]
         * \return std::string
         */
        [[nodiscard]] auto toString() const -> std::string;

        /**
         * \brief Creates Date object which represents local date.
         * \return DateTime.
         */
        [[nodiscard]] static auto localDateTime() -> DateTime;

    protected:
    private:
        inline static Formatter m_formatter_global;
        Formatter m_formatter_local;

        date::Date m_date;
        time::Time m_time;
    };

    /**
     * \brief Operator !=
     * \param l const DateTime &
     * \param r const DateTime &
     * \return bool
     */
    auto operator!=(const DateTime& l, const DateTime& r) -> bool;
    /**
     * \brief Operator >
     * \param l const DateTime &
     * \param r const DateTime &
     * \return bool
     */
    auto operator>(const DateTime& l, const DateTime& r) -> bool;
    /**
     * \brief Operator <=
     * \param l const DateTime &
     * \param r const DateTime &
     * \return bool
     */
    auto operator<=(const DateTime& l, const DateTime& r) -> bool;
    /**
     * \brief Operator >=
     * \param l const DateTime &
     * \param r const DateTime &
     * \return bool
     */
    auto operator>=(const DateTime& l, const DateTime& r) -> bool;
    /**
     * \brief Operator <<
     * \param out std::ostream&
     * \param date const DateTime&
     * \return std::ostream&
     * \note Method toString() is used here
     */
    auto operator<<(std::ostream& out, const DateTime& dt) -> std::ostream&;

}  // namespace tristan::date_time

#endif  // DATE_TIME_HPP
