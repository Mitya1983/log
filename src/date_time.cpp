#include "date_time.hpp"

namespace {

    auto g_default_global_formatter = [](const tristan::date_time::DateTime& p_date) -> std::string {
        std::string dt;
        dt += p_date.date().toString();
        dt += 'T';
        dt += p_date.time().toString();
        return dt;
    };

}  //End of anonymous namespace

tristan::date_time::DateTime::DateTime(tristan::time::Precision p_precision) :
    m_date(),
    m_time(p_precision) { }

tristan::date_time::DateTime::DateTime(tristan::TimeZone p_time_zone, tristan::time::Precision p_precision) :
    m_date(p_time_zone),
    m_time(p_time_zone, p_precision) { }

tristan::date_time::DateTime::DateTime(const std::string& date) {
    auto delimiter_pos = date.find('T');
    if (delimiter_pos == std::string::npos) {
        throw std::runtime_error("tristan::date_time::DateTime::DateTime(const std::string& time): Invalid time format");
    }
    m_date = tristan::date::Date(date.substr(0, delimiter_pos));
    m_time = tristan::time::Time(date.substr(delimiter_pos + 1));
}

void tristan::date_time::DateTime::setDate(const tristan::date::Date& p_date) { m_date = p_date; }

void tristan::date_time::DateTime::setDate(tristan::date::Date&& p_date) { m_date = std::move(p_date); }

void tristan::date_time::DateTime::setTime(const tristan::time::Time& p_time) { m_time = p_time; }

void tristan::date_time::DateTime::setTime(tristan::time::Time&& p_time) { m_time = std::move(p_time); }

auto tristan::date_time::DateTime::date() const -> const tristan::date::Date& { return m_date; }

auto tristan::date_time::DateTime::time() const -> const tristan::time::Time& { return m_time; }

void tristan::date_time::DateTime::setTimeLocalFormatter(tristan::time::Formatter&& p_formatter) { m_time.setLocalFormatter(std::move(p_formatter)); }

void tristan::date_time::DateTime::setDateLocalFormatter(tristan::date::Formatter&& p_formatter) { m_date.setLocalFormatter(std::move(p_formatter)); }

void tristan::date_time::DateTime::setLocalFormatter(tristan::date_time::Formatter&& p_formatter) { m_formatter_local = std::move(p_formatter); }

void tristan::date_time::DateTime::setGlobalFormatter(tristan::date_time::Formatter&& p_formatter) { m_formatter_global = std::move(p_formatter); }

auto tristan::date_time::DateTime::toString() const -> std::string {
    if (not m_formatter_global) {
        tristan::date_time::DateTime::m_formatter_global = g_default_global_formatter;
    }
    if (m_formatter_local) {
        return m_formatter_local(*this);
    }
    return m_formatter_global(*this);
}

auto tristan::date_time::DateTime::localDateTime() -> tristan::date_time::DateTime {
    tristan::date_time::DateTime l_date_time;
    l_date_time.setDate(tristan::date::Date::localDate());
    l_date_time.setTime(tristan::time::Time::localTime());

    return l_date_time;
}

auto tristan::date_time::DateTime::operator==(const tristan::date_time::DateTime& other) const -> bool {
    return m_date == other.m_date && m_time == other.m_time;
}

auto tristan::date_time::DateTime::operator<(const tristan::date_time::DateTime& other) const -> bool {
    if (m_date > other.m_date) {
        return false;
    }
    if (m_date == other.m_date) {
        if (m_time >= other.m_time) {
            return false;
        }
    }
    return true;
}

auto tristan::date_time::operator<<(std::ostream& out, const tristan::date_time::DateTime& dt) -> std::ostream& {
    out << dt.toString();
    return out;
}

auto tristan::date_time::operator!=(const tristan::date_time::DateTime& l, const tristan::date_time::DateTime& r) -> bool { return !(l == r); }

auto tristan::date_time::operator>(const tristan::date_time::DateTime& l, const tristan::date_time::DateTime& r) -> bool { return !(l <= r); }

auto tristan::date_time::operator<=(const tristan::date_time::DateTime& l, const tristan::date_time::DateTime& r) -> bool { return (l < r || l == r); }

auto tristan::date_time::operator>=(const tristan::date_time::DateTime& l, const tristan::date_time::DateTime& r) -> bool { return (l > r || l == r); }
