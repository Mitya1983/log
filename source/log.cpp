#include "log.hpp"
#include <thread>

#if (defined __linux) || (defined linux) || (defined __linux__) || (defined __OSX__) || (defined __APPLE__)
  #include <unistd.h>
#else
  #define NOMINMAX
  #include <windows.h>
  #include <cerrno>
#endif

#if defined __cpp_lib_format
  #include <format>
#endif
using namespace mt::log;

auto mt::log::processID() -> uint64_t {
#if (defined __linux) || (defined linux) || (defined __linux__)
    return static_cast< uint64_t >(getpid());
#else
    return static_cast< uint64_t >(GetCurrentProcessId());
#endif
}

LogEvent::LogEvent(std::string p_message, const MessageType p_message_type, std::string p_function_name, std::string p_file_name, const uint32_t p_line) :
    time_point(std::chrono::system_clock::now()),
    message(std::move(p_message)),
    function_name(std::move(p_function_name)),
    file_name(std::move(p_file_name)),
    line(std::to_string(p_line)),
    thread_id(std::this_thread::get_id()),
    message_type(p_message_type) { }

LogEvent::LogEvent(std::string p_message, const MessageType p_message_type, const std::source_location p_source_location) :
    LogEvent(std::move(p_message), p_message_type, p_source_location.function_name(), p_source_location.file_name(), p_source_location.line()) { }

LogEvent::LogEvent(std::string_view p_message, MessageType p_message_type, std::string p_function_name, std::string p_file_name, uint32_t p_line) :
    LogEvent(std::string{p_message}, p_message_type, std::move(p_function_name), std::move(p_file_name), p_line) { }

LogEvent::LogEvent(std::string_view p_message, MessageType p_message_type, std::source_location p_source_location) :
    LogEvent(std::string{p_message}, p_message_type, p_source_location) { }

LogEvent::LogEvent(const char* p_message, MessageType p_message_type, std::string p_function_name, std::string p_file_name, uint32_t p_line) :
    LogEvent(std::string_view{p_message}, p_message_type, std::move(p_function_name), std::move(p_file_name), p_line) { }

LogEvent::LogEvent(const char* p_message, MessageType p_message_type, std::source_location p_source_location) :
    LogEvent(std::string_view{p_message}, p_message_type, p_source_location) { }

LogEvent::~LogEvent() = default;

auto LogEvent::toString(const std::function< std::string(const LogEvent&) >& formatter) const -> std::string {
    if (formatter) {
        return formatter(*this);
    }
#if defined __cpp_lib_format
    return std::format("{}|{}|{}|{}|{}|{}|{}\n", time_point, message_type_string, module_name, message, function_name, file_name, line);
#endif
    const auto time = std::chrono::system_clock::to_time_t(time_point);
    auto [tm_sec, tm_min, tm_hour, tm_mday, tm_mon, tm_year, tm_wday, tm_yday, tm_isdst, tm_gmtoff, tm_zone] = *std::gmtime(&time);
    std::string string_time = std::to_string(tm_year + 1900);
    string_time += '-';
    string_time += std::to_string(tm_mon + 1);
    string_time += '-';
    string_time += std::to_string(tm_mday);
    string_time += 'T';
    string_time += std::to_string(tm_hour);
    string_time += ':';
    string_time += std::to_string(tm_min);
    string_time += ':';
    string_time += std::to_string(tm_sec);
    return {string_time + " | " + message_type_string + " | " + module_name + " | " + message + " | " + function_name + " | " + file_name + " | " + line
            + '\n'};
}