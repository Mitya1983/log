#include "date.hpp"
#include <algorithm>

namespace {

    inline constexpr uint16_t g_non_leap_year_days{365};
    inline constexpr uint16_t g_leap_year_days{366};
    inline constexpr uint16_t g_days_since_1900_to_1970{25567};
    inline constexpr uint16_t g_start_year = 1900;

    using Years = std::chrono::duration< int64_t, std::ratio_divide< std::ratio< 31536000 >, std::chrono::seconds::period > >;

    inline constexpr uint8_t g_february_start_day = 32;
    inline constexpr std::pair< uint8_t, uint8_t > g_march_start_day = {60, 61};
    inline constexpr std::pair< uint8_t, uint8_t > g_april_start_day = {91, 92};
    inline constexpr std::pair< uint8_t, uint8_t > g_may_start_day = {121, 122};
    inline constexpr std::pair< uint8_t, uint8_t > g_june_start_day = {152, 153};
    inline constexpr std::pair< uint8_t, uint8_t > g_july_start_day = {182, 183};
    inline constexpr std::pair< uint8_t, uint8_t > g_august_start_day = {213, 214};
    inline constexpr std::pair< uint8_t, uint8_t > g_september_start_day = {244, 245};
    inline constexpr std::pair< uint16_t, uint16_t > g_october_start_day = {274, 275};
    inline constexpr std::pair< uint16_t, uint16_t > g_november_start_day = {305, 306};
    inline constexpr std::pair< uint16_t, uint16_t > g_december_start_day = {335, 336};

    enum Months : uint8_t {
        JANUARY = 1,
        FEBRUARY,
        MARCH,
        APRIL,
        MAY,
        JUNE,
        JULY,
        AUGUST,
        SEPTEMBER,
        OCTOBER,
        NOVEMBER,
        DECEMBER
    };

    auto g_default_global_formatter = [](const tristan::date::Date& p_date) -> std::string {
        std::string result;
        result += std::to_string(p_date.year());
        result += '-';
        uint8_t month = p_date.month();
        if (month < 10)
            result += '0';
        result += std::to_string(month);
        result += '-';
        uint8_t day = p_date.dayOfTheMonth();
        if (day < 10)
            result += '0';
        result += std::to_string(day);

        return result;
    };
}  //End of unnamed namespace

tristan::date::Date::Date() :
    m_days_since_1900(Days{g_days_since_1900_to_1970}) {
    auto time_since_epoch
        = std::chrono::duration_cast< std::chrono::seconds >(std::chrono::system_clock::duration(std::chrono::system_clock::now().time_since_epoch()));
    m_days_since_1900 += std::chrono::duration_cast< Days >(time_since_epoch);
    m_days_since_1900 += Days{1};
}

tristan::date::Date::Date(tristan::TimeZone p_time_zone) :
    m_days_since_1900(Days{g_days_since_1900_to_1970}) {
    auto time_since_epoch
        = std::chrono::duration_cast< std::chrono::seconds >(std::chrono::system_clock::duration(std::chrono::system_clock::now().time_since_epoch()));
    time_since_epoch += std::chrono::duration_cast< std::chrono::seconds >(std::chrono::hours(static_cast< int8_t >(p_time_zone)));
    m_days_since_1900 += std::chrono::duration_cast< Days >(time_since_epoch);
    m_days_since_1900 += Days{1};
}

tristan::date::Date::Date(uint8_t day, uint8_t month, uint16_t year) {
    if (year < g_start_year) {
        std::string message = "tristan::date::Date(int year, int month, int day): bad [year] value was provided - " + std::to_string(day)
                              + " the value between 1900 and 2155 is expected";
        throw std::range_error(message);
    }
    if (day < 1 || day > 31) {
        std::string message = "tristan::date::Date(int year, int month, int day): bad [day] value was provided - " + std::to_string(day)
                              + " the value between 0 and 31 is expected";
        throw std::range_error(message);
    }
    if (month < 1 || month > 12) {
        std::string message = "tristan::date::Date(int year, int month, int day): bad [month] value was provided - " + std::to_string(month)
                              + " the value between 0 and 12 is expected";
        throw std::range_error(message);
    }
    if ((month == 4 || month == 6 || month == 9 || month == 11) && day == 31) {
        std::string message = "tristan::date::Date(int year, int month, int day): date 31 is not possible for provided month " + std::to_string(month);
        throw std::range_error(message);
    } else if (month == 2) {
        bool leap_year = tristan::date::Date::isLeapYear(year);
        if ((leap_year && day > 29) || (!leap_year && day > 28)) {
            std::string message = "tristan::date::Date(int year, int month, int day): date " + std::to_string(day) + " is not possible for provided month "
                                  + std::to_string(month);
            throw std::range_error(message);
        }
    }
    m_days_since_1900 = std::chrono::duration_cast< Days >(Years{year - g_start_year});
    m_days_since_1900 += Days{static_cast< uint8_t >(_calculateCurrentYear() * 365 / (g_non_leap_year_days * 4))};
    //    if (tristan::date::Date::isLeapYear(year)){
    //        m_days_since_1900 -= tristan::date::Days{1};
    //    }
    for (int l_month = 1; l_month < month; ++l_month) {
        if (l_month == 1 || l_month == 3 || l_month == 5 || l_month == 7 || l_month == 8 || l_month == 10 || l_month == 12) {
            m_days_since_1900 += Days{31};
        } else if (l_month == 4 || l_month == 6 || l_month == 9 || l_month == 11) {
            m_days_since_1900 += Days{30};
        } else {
            bool leap_year = isLeapYear(std::chrono::duration_cast< Years >(m_days_since_1900).count() + g_start_year);
            if (!leap_year) {
                m_days_since_1900 += Days{28};
            } else {
                m_days_since_1900 += Days{29};
            }
        }
    }
    m_days_since_1900 += Days{day};
}

tristan::date::Date::Date(const std::string& iso_date) {
    auto _length = iso_date.length();
    if (_length != 8 && _length != 10) {
        throw std::invalid_argument("Provided date has invalid format. "
                                    "Value should be provided as either 8 (YYYYMMDD) or 10 (YYYY-MM-DD) characters string. "
                                    "But "
                                    + std::to_string(_length) + " characters string was provided");
    }
    if (!(std::find_if(iso_date.begin(),
                       iso_date.end(),
                       [](const char c) {
                           if (c < '0' || c > '9') {
                               if (c != '-') {
                                   return true;
                               }
                           }
                           return false;
                       })
          != iso_date.end())) {
        uint16_t year = static_cast< uint16_t >(std::stoi(iso_date.substr(0, 4)));
        uint8_t month = 0;
        uint8_t day = 0;

        switch (_length) {
            case 8: {
                month = std::stoi(iso_date.substr(4, 2));
                day = std::stoi(iso_date.substr(6, 2));
                break;
            }
            case 10: {
                month = std::stoi(iso_date.substr(5, 2));
                day = std::stoi(iso_date.substr(8, 2));
                break;
            }
            default: {
                break;
            }
        }
        *this = tristan::date::Date(day, month, year);
    } else {
        std::string message = "tristan::date::Date::Date(const std::string &iso_date): Bad [iso_date] string "
                              "format. String should contain only numbers and hyphen";
        throw std::invalid_argument(message);
    }
}

auto tristan::date::Date::operator==(const tristan::date::Date& other) const -> bool { return m_days_since_1900 == other.m_days_since_1900; }

auto tristan::date::Date::operator<(const tristan::date::Date& other) const -> bool { return m_days_since_1900 < other.m_days_since_1900; }

void tristan::date::Date::addDays(uint64_t days) {
    if (days == 0) {
        return;
    }
    m_days_since_1900 += Days{days};
}

void tristan::date::Date::addMonths(uint64_t months) {
    if (months == 0) {
        return;
    }
    uint64_t days_to_add = 0;
    uint8_t current_month = _calculateCurrentMonth();
    uint8_t current_date = _calculateDayOfTheMonth();
    for (uint64_t l_month = 0; l_month < months; ++l_month) {
        if (current_month == 13) {
            this->addDays(days_to_add);
            days_to_add = 0;
            current_month = _calculateCurrentMonth();
            current_date = _calculateDayOfTheMonth();
        }
        if ((current_month == Months::JANUARY && current_date < 29) || (current_month == Months::MARCH && current_date < 31)
            || (current_month == Months::MAY && current_date < 31) || current_month == Months::JULY || (current_month == Months::AUGUST && current_date < 31)
            || (current_month == Months::OCTOBER && current_date < 31) || current_month == Months::DECEMBER) {
            days_to_add += 31;
        } else if (current_month == Months::FEBRUARY || current_month == Months::APRIL || current_month == Months::JUNE || current_month == Months::SEPTEMBER
                   || current_month == Months::NOVEMBER || (current_month == Months::MARCH && current_date == 31)
                   || (current_month == Months::MAY && current_date == 31) || (current_month == Months::AUGUST && current_date == 31)
                   || (current_month == Months::OCTOBER && current_date == 31)) {
            days_to_add += 30;
        } else if (current_month == JANUARY && current_date >= 29) {
            bool leap_year = tristan::date::Date::isLeapYear(_calculateCurrentYear() + g_start_year);
            if (!leap_year) {
                switch (current_date) {
                    case 29: {
                        days_to_add += 30;
                        break;
                    }
                    case 30: {
                        days_to_add += 29;
                        break;
                    }
                    case 31: {
                        days_to_add += 28;
                        break;
                    }
                    default: {
                        break;
                    }
                }
            } else {
                switch (current_date) {
                    case 29: {
                        days_to_add += 31;
                        break;
                    }
                    case 30: {
                        days_to_add += 30;
                        break;
                    }
                    case 31: {
                        days_to_add += 29;
                        break;
                    }
                    default: {
                        break;
                    }
                }
            }
        }
        ++current_month;
    }
    this->addDays(days_to_add);
}

void tristan::date::Date::addYears(uint64_t years) {
    if (years == 0) {
        return;
    }
    uint64_t days_to_add = 0;
    uint16_t days_in_year = _calculateDaysInYear();
    for (uint64_t year = _calculateCurrentYear() + g_start_year, end_year = year + years; year < end_year; ++year) {
        if (days_in_year < 60) {
            if (tristan::date::Date::isLeapYear(year)) {
                days_to_add += 366;
            } else {
                days_to_add += 364;
            }
        } else if (days_in_year >= 60) {
            if (tristan::date::Date::isLeapYear(year + 1)) {
                days_to_add += 366;
            } else {
                days_to_add += 365;
            }
        }
    }
    this->addDays(days_to_add);
}

void tristan::date::Date::subtractDays(uint64_t days) {
    if (days == 0) {
        return;
    }
    m_days_since_1900 -= Days{days};
}

void tristan::date::Date::subtractMonths(uint64_t months) {
    if (months == 0) {
        return;
    }
    uint8_t current_month = _calculateCurrentMonth();
    uint8_t current_date = _calculateDayOfTheMonth();
    uint64_t days_to_subtract = 0;
    for (uint64_t l_month = 0; l_month < months; ++l_month) {
        if (current_month == 0) {
            this->subtractDays(days_to_subtract);
            days_to_subtract = 0;
            current_month = _calculateCurrentMonth();
            current_date = _calculateDayOfTheMonth();
        }
        if (current_month == Months::JANUARY || (current_month == Months::DECEMBER && current_date == 31) || current_month == Months::NOVEMBER
            || (current_month == Months::OCTOBER && current_date == 31) || current_month == Months::SEPTEMBER || current_month == Months::AUGUST
            || (current_month == Months::JULY && current_date == 31) || current_month == Months::JUNE || (current_month == Months::MAY && current_date == 31)
            || current_month == Months::FEBRUARY) {
            days_to_subtract += 31;
        } else if (current_month == Months::DECEMBER || current_month == Months::OCTOBER || current_month == Months::JULY || current_month == Months::MAY) {
            days_to_subtract += 30;
        } else if (current_month == Months::MARCH) {
            if (current_date > 28) {
                days_to_subtract += current_date;
            } else {
                bool is_leap_year = tristan::date::Date::isLeapYear(_calculateCurrentYear() + g_start_year);
                if (!is_leap_year) {
                    days_to_subtract += 28;
                } else {
                    days_to_subtract += 29;
                }
            }
        }
        --current_month;
    }
    this->subtractDays(days_to_subtract);
}

void tristan::date::Date::subtractYears(uint64_t years) {
    if (years == 0) {
        return;
    }
    uint64_t days_to_subtract = 0;
    uint16_t days_in_year = _calculateDaysInYear();
    for (uint64_t year = _calculateCurrentYear() + g_start_year, end_year = year - years; year > end_year; --year) {
        if (tristan::date::Date::isLeapYear(year)) {
            if (days_in_year < 60) {
                days_to_subtract += g_non_leap_year_days;
            } else {
                days_to_subtract += g_leap_year_days;
            }
        } else if (tristan::date::Date::isLeapYear(year - 1)) {
            if (days_in_year <= 59) {
                days_to_subtract += g_leap_year_days;
            } else {
                days_to_subtract += g_non_leap_year_days;
            }
        } else {
            days_to_subtract += g_non_leap_year_days;
        }
    }
    this->subtractDays(days_to_subtract);
}

auto tristan::date::Date::dayOfTheMonth() const -> uint8_t { return _calculateDayOfTheMonth(); }

auto tristan::date::Date::dayOfTheWeek() const -> uint8_t { return (m_days_since_1900.count()) % 7; }

auto tristan::date::Date::month() const -> uint8_t { return _calculateCurrentMonth(); }

auto tristan::date::Date::year() const -> uint16_t { return _calculateCurrentYear() + g_start_year; }

auto tristan::date::Date::isWeekend() const -> bool { return this->dayOfTheWeek() > 5; }

bool tristan::date::Date::isLeapYear(uint16_t year) {
    if (year % 4 == 0) {
        if (year % 100 == 0) {
            if (year % 400 == 0) {
                return true;
            } else {
                return false;
            }
        } else {
            return true;
        }
    } else {
        return false;
    }
}

void tristan::date::Date::setGlobalFormatter(tristan::date::Formatter&& p_formatter) { m_formatter_global = std::move(p_formatter); }

void tristan::date::Date::setLocalFormatter(tristan::date::Formatter&& p_formatter) { m_formatter_local = std::move(p_formatter); }

std::string tristan::date::Date::toString() const {
    if (not m_formatter_global) {
        tristan::date::Date::m_formatter_global = g_default_global_formatter;
    }
    if (m_formatter_local) {
        return m_formatter_local(*this);
    }
    return m_formatter_global(*this);
}

auto tristan::date::Date::localDate() -> tristan::date::Date {
    auto tm = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto offset = std::localtime(&tm)->tm_gmtoff;
    return tristan::date::Date(static_cast< tristan::TimeZone >(offset / 3600));
}

auto tristan::date::Date::_calculateCurrentMonth() const -> uint8_t {
    uint16_t days_since_year_start = _calculateDaysInYear();
    bool leap_year = tristan::date::Date::isLeapYear(_calculateCurrentYear() + g_start_year);
    if ((!leap_year && days_since_year_start >= g_december_start_day.first) || (leap_year && days_since_year_start >= g_december_start_day.second)) {
        return Months::DECEMBER;
    } else if ((!leap_year && days_since_year_start >= g_november_start_day.first) || (leap_year && days_since_year_start >= g_november_start_day.second)) {
        return Months::NOVEMBER;
    } else if ((!leap_year && days_since_year_start >= g_october_start_day.first) || (leap_year && days_since_year_start >= g_october_start_day.second)) {
        return Months::OCTOBER;
    } else if ((!leap_year && days_since_year_start >= g_september_start_day.first) || (leap_year && days_since_year_start >= g_september_start_day.second)) {
        return Months::SEPTEMBER;
    } else if ((!leap_year && days_since_year_start >= g_august_start_day.first) || (leap_year && days_since_year_start >= g_august_start_day.second)) {
        return Months::AUGUST;
    } else if ((!leap_year && days_since_year_start >= g_july_start_day.first) || (leap_year && days_since_year_start >= g_july_start_day.second)) {
        return Months::JULY;
    } else if ((!leap_year && days_since_year_start >= g_june_start_day.first) || (leap_year && days_since_year_start >= g_june_start_day.second)) {
        return Months::JUNE;
    } else if ((!leap_year && days_since_year_start >= g_may_start_day.first) || (leap_year && days_since_year_start >= g_may_start_day.second)) {
        return Months::MAY;
    } else if ((!leap_year && days_since_year_start >= g_april_start_day.first) || (leap_year && days_since_year_start >= g_april_start_day.second)) {
        return Months::APRIL;
    } else if ((!leap_year && days_since_year_start >= g_march_start_day.first) || (leap_year && days_since_year_start >= g_march_start_day.second)) {
        return Months::MARCH;
    } else if (days_since_year_start >= g_february_start_day) {
        return Months::FEBRUARY;
    } else {
        return Months::JANUARY;
    }
}

auto tristan::date::Date::_calculateCurrentYear() const -> uint8_t {
    auto number_of_leap_days = static_cast< uint8_t >(m_days_since_1900.count() / (g_non_leap_year_days * 4));

    return (m_days_since_1900.count() - number_of_leap_days) / g_non_leap_year_days;
}

auto tristan::date::Date::_calculateDayOfTheMonth() const -> uint8_t {

    uint16_t days_in_month = _calculateDaysInYear();
    for (int month = _calculateCurrentMonth() - 1; month >= Months::JANUARY; --month) {
        if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) {
            days_in_month -= 31;
        } else if (month == 4 || month == 6 || month == 9 || month == 11) {
            days_in_month -= 30;
        } else {
            bool leap_year = isLeapYear(_calculateCurrentYear() + g_start_year);
            if (!leap_year) {
                days_in_month -= 28;
            } else {
                days_in_month -= 29;
            }
        }
    }
    return days_in_month;
}

auto tristan::date::Date::_calculateDaysInYear() const -> uint16_t {
    auto number_of_leap_days = static_cast< uint8_t >(m_days_since_1900.count() / (g_non_leap_year_days * 4));
    if (tristan::date::Date::isLeapYear(_calculateCurrentYear() + g_start_year)) {
        number_of_leap_days -= 1;
    }
    uint8_t years = _calculateCurrentYear();
    return static_cast< uint16_t >(m_days_since_1900.count() - years * g_non_leap_year_days - number_of_leap_days);
}

bool tristan::date::operator!=(const tristan::date::Date& l, const tristan::date::Date& r) { return !(l == r); }

bool tristan::date::operator>(const tristan::date::Date& l, const tristan::date::Date& r) { return !(l < r); }

bool tristan::date::operator<=(const tristan::date::Date& l, const tristan::date::Date& r) { return l < r || l == r; }

bool tristan::date::operator>=(const tristan::date::Date& l, const tristan::date::Date& r) { return l > r || l == r; }

std::ostream& tristan::date::operator<<(std::ostream& out, const tristan::date::Date& date) {
    out << date.toString();
    return out;
}
