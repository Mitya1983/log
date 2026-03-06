#include "log.hpp"
#include <thread>

#if (defined __linux) or (defined linux) or (defined __linux__) or (defined __OSX__) or (defined __APPLE__)
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
#if defined(__linux) || defined(linux) || defined(__linux__) || defined(__APPLE__)
    return static_cast< uint64_t >(getpid());
#else
    return static_cast< uint64_t >(GetCurrentProcessId());
#endif
}

log_event::log_event(std::string p_module,
                     std::string p_message,
                     const enum message_type p_message_type,
                     std::string p_function_name,
                     std::string p_file_name,
                     const uint32_t p_line) :
    time_point(std::chrono::system_clock::now()),
    module_name(std::move(p_module)),
    message(std::move(p_message)),
    function_name(std::move(p_function_name)),
    file_name(std::move(p_file_name)),
    line(std::to_string(p_line)),
    message_type(p_message_type) { }

log_event::log_event(std::string p_module, std::string p_message, const enum message_type p_message_type, const std::source_location p_source_location) :
    log_event(std::move(p_module),
              std::move(p_message),
              p_message_type,
              p_source_location.function_name(),
              p_source_location.file_name(),
              p_source_location.line()) { }

log_event::log_event(const std::string_view p_module,
                     const std::string_view p_message,
                     const enum message_type p_message_type,
                     std::string p_function_name,
                     std::string p_file_name,
                     const uint32_t p_line) :
    log_event(std::string{p_module}, std::string{p_message}, p_message_type, std::move(p_function_name), std::move(p_file_name), p_line) { }

log_event::log_event(const std::string_view p_module,
                     const std::string_view p_message,
                     const enum message_type p_message_type,
                     const std::source_location p_source_location) :
    log_event(std::string{p_module}, std::string{p_message}, p_message_type, p_source_location) { }

auto log_event::to_string(const std::function< std::string(const log_event&) >& formatter) const -> std::string {
    if (formatter) {
        return formatter(*this);
    }
#if defined __cpp_lib_format and not defined(__APPLE__)
    return std::format("{}|{}|{}|{}|{}|{}|{}\n", time_point, message_type_string, module_name, message, function_name, file_name, line);
#else
    const auto time = std::chrono::system_clock::to_time_t(time_point);
    std::tm tm_struct{};
  #if defined(_WIN32) || defined(_WIN64)
    gmtime_s(&tm_struct, &time);
  #else
    gmtime_r(&time, &tm_struct);
  #endif
    std::string string_time = std::to_string(tm_struct.tm_year + 1900);
    string_time += '-';
    string_time += std::to_string(tm_struct.tm_mon + 1);
    string_time += '-';
    string_time += std::to_string(tm_struct.tm_mday);
    string_time += 'T';
    string_time += std::to_string(tm_struct.tm_hour);
    string_time += ':';
    string_time += std::to_string(tm_struct.tm_min);
    string_time += ':';
    string_time += std::to_string(tm_struct.tm_sec);
    return {string_time + " | " + message_type_string + " | " + module_name + " | " + message + " | " + function_name + " | " + file_name + " | " + line
            + '\n'};
#endif
}
