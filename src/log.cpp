#include "log.hpp"
#include "ipc_lock.hpp"

#include <fstream>
#include <iostream>
#include <ctime>

#if defined(_WIN32) || defined(_WIN64)
  #include <cerrno>
#endif

#include <thread>

using namespace tristan::log;

namespace {

    /**
     * \private
     * \brief Default formatter function for Trace messages.
     * \param time_point const std::chrono::time_point<std::chrono::system_clock>&
     * \param message const std::string& message
     * \param message_type const std::string& message
     * \param function_name const std::string& message
     * \param file_name const std::string& message
     * \param line int
     * \return std::string which stores formatted message which will be sent to output
     */
    auto traceFormatter(const LogEvent& log_event) -> std::string;
    /**
     * \private
     * \brief Default formatter function for Debug messages.
     * \param time_point const std::chrono::time_point<std::chrono::system_clock>&
     * \param message const std::string& message
     * \param message_type const std::string& message
     * \param function_name const std::string& message
     * \param file_name const std::string& message
     * \param line int
     * \return std::string which stores formatted message which will be sent to output
     */
    auto debugFormatter(const LogEvent& log_event) -> std::string;
    /**
     * \private
     * \brief Default formatter function for Error messages.
     * \param time_point const std::chrono::time_point<std::chrono::system_clock>&
     * \param message const std::string& message
     * \param message_type const std::string& message
     * \param function_name const std::string& message
     * \param file_name const std::string& message
     * \param line int
     * \return std::string which stores formatted message which will be sent to output
     */
    auto errorFormatter(const LogEvent& log_event) -> std::string;
    /**
     * \private
     * \brief Default formatter function for Warning messages.
     * \param time_point const std::chrono::time_point<std::chrono::system_clock>&
     * \param message const std::string& message
     * \param message_type const std::string& message
     * \param function_name const std::string& message
     * \param file_name const std::string& message
     * \param line int
     * \return std::string which stores formatted message which will be sent to output
     */
    auto warningFormatter(const LogEvent& log_event) -> std::string;
    /**
     * \private
     * \brief Default formatter function for Info messages.
     * \param time_point const std::chrono::time_point<std::chrono::system_clock>&
     * \param message const std::string& message
     * \param message_type const std::string& message
     * \param function_name const std::string& message
     * \param file_name const std::string& message
     * \param line int
     * \return std::string which stores formatted message which will be sent to output
     */
    auto infoFormatter(const LogEvent& log_event) -> std::string;
    /**
     * \private
     * \brief Default formatter function for Fatal messages.
     * \param time_point const std::chrono::time_point<std::chrono::system_clock>&
     * \param message const std::string& message
     * \param message_type const std::string& message
     * \param function_name const std::string& message
     * \param file_name const std::string& message
     * \param line int
     * \return std::string which stores formatted message which will be sent to output
     */
    auto fatalFormatter(const LogEvent& log_event) -> std::string;

    /**
     * \brief Used to handle field width which is set by default formatter using std::setw call.
     */
    inline uint8_t g_message_type_output_width = 7;

}  // End of unnamed namespace

Log::Log() :
    m_ipc_lock_name("LoggerLock") {

    m_message_types.emplace_back("TRACE");
    m_message_types.emplace_back("DEBUG");
    m_message_types.emplace_back("ERROR");
    m_message_types.emplace_back("WARNING");
    m_message_types.emplace_back("INFO");
    m_message_types.emplace_back("FATAL");

    m_outputs.emplace_back(&std::cout);
    m_outputs.emplace_back(&std::cout);
    m_outputs.emplace_back(&std::cout);
    m_outputs.emplace_back(&std::cout);
    m_outputs.emplace_back(&std::cout);
    m_outputs.emplace_back(&std::cout);

    m_formatters.emplace_back(traceFormatter);
    m_formatters.emplace_back(debugFormatter);
    m_formatters.emplace_back(errorFormatter);
    m_formatters.emplace_back(warningFormatter);
    m_formatters.emplace_back(infoFormatter);
    m_formatters.emplace_back(fatalFormatter);
}

void Log::setModuleName(std::string name) { m_module_name = std::move(name); }

void Log::setIpcLockName(std::string name) { m_ipc_lock_name = std::move(name); }

void Log::setMessageTypeOutput(MessageType message_type, const std::string& value) {
    m_message_types.at(static_cast< size_t >(message_type)) = value;
    if (value.length() > g_message_type_output_width) {
        g_message_type_output_width = value.length();
    }
}

void Log::setGlobalOutput(std::ostream* output_stream) {
    for (auto& output: m_outputs) {
        if (output_stream == nullptr) {
            output = std::monostate();
        } else {
            output = output_stream;
        }
    }
}

void Log::setGlobalOutput(const std::filesystem::path& file) {
    for (auto& output: m_outputs) {
        output = file;
    }
}

void Log::setGlobalOutput(std::function< void(const std::string&) >&& output_func) {
    for (auto& l_output: m_outputs) {
        if (output_func == nullptr) {
            l_output = std::monostate();
        } else {
            l_output = output_func;
        }
    }
}

void Log::setOutput(MessageType message_type, std::ostream* output_stream) {
    if (output_stream == nullptr) {
        m_outputs.at(static_cast< size_t >(message_type)) = std::monostate();
    } else {
        m_outputs.at(static_cast< size_t >(message_type)) = output_stream;
    }
}

void Log::setOutput(MessageType message_type, const std::filesystem::path& file) {
    m_outputs.at(static_cast< size_t >(message_type)) = file;
}

void Log::setOutput(MessageType message_type, std::function< void(const std::string&) >&& output_func) {
    if (output_func == nullptr) {
        m_outputs.at(static_cast< size_t >(message_type)) = std::monostate();
    } else {
        m_outputs.at(static_cast< size_t >(message_type)) = output_func;
    }
}

void Log::setGlobalFormatter(std::function< std::string(LogEvent&& log_event) >&& formatter) {
    for (auto& l_formatter: m_formatters) {
        l_formatter = formatter;
    }
}

void Log::setFormatter(MessageType message_type,
                       std::function< std::string(LogEvent&& log_event) >&& formatter) {
    m_formatters.at(static_cast< size_t >(message_type)) = formatter;
}

void Log::write(LogEvent&& log_event) {

#if defined(LOG_DISABLE_TRACE)
    if (message_type == MessageType::Trace) {
        return;
    }
#endif
#if defined(LOG_DISABLE_DEBUG)
    if (message_type == MessageType::Debug) {
        return;
    }
#endif
#if defined(LOG_DISABLE_ERROR)
    if (message_type == MessageType::Error) {
        return;
    }
#endif
#if defined(LOG_DISABLE_WARNING)
    if (message_type == MessageType::Warning) {
        return;
    }
#endif
#if defined(LOG_DISABLE_INFO)
    if (message_type == MessageType::Info) {
        return;
    }
#endif
#if defined(LOG_DISABLE_FATAL)
    if (message_type == MessageType::Fatal) {
        return;
    }
#endif

    auto message_type_index = static_cast< size_t >(log_event.message_type);
    log_event.message_type_string = m_message_types.at(message_type_index);
    log_event.module_name = m_module_name;
    auto formatter = m_formatters.at(message_type_index);
    std::string msg = formatter(std::move(log_event));
    std::visit(
        [this, &msg](auto&& arg) -> void {
            using T = std::decay_t< decltype(arg) >;
            if constexpr (std::is_same_v< T, std::ostream* >) {
                std::scoped_lock< std::mutex > lock(m_mutex);
                *arg << msg << std::endl;
            } else if constexpr (std::is_same_v< T, std::filesystem::path >) {
                std::scoped_lock< std::mutex > lock(m_mutex);
                tristan::IPC_Lock file_lock(m_ipc_lock_name);
                std::ofstream file(arg, std::ios::app);
                if (not file.is_open()) {
                    throw std::fstream::failure("Could not open Log file for writing - ",
                                                std::error_code(errno, std::system_category()));
                }
                file << msg << std::endl;
                file.close();
            } else if constexpr (std::is_same_v< T, std::function< void(const std::string&) > >) {
                arg(msg);
            }
        },
        m_outputs.at(message_type_index));
}

auto Log::createLogInstance() -> std::unique_ptr< Log > { return std::unique_ptr< Log >(new Log()); }

namespace {

    auto traceFormatter(const LogEvent& log_event) -> std::string {
        std::stringstream output;
        output << log_event.thread_id << " | " << log_event.time_point.time_since_epoch().count() << " | "
               << log_event.module_name << " | " << log_event.function_name << " | " << log_event.message;
        return output.str();
    }

    auto debugFormatter(const LogEvent& log_event) -> std::string {
        std::stringstream output;
        tm tm_time{};
        std::time_t time = std::chrono::system_clock::to_time_t(log_event.time_point);
        gmtime_r(const_cast< const std::time_t* >(&time), &tm_time);
        output << std::put_time(const_cast< const tm* >(&tm_time), "%FT%T%Z") << " | " << std::left
               << std::setw(g_message_type_output_width) << log_event.message_type_string << " | "
               << log_event.module_name << " | "
               << "MESSAGE: " << log_event.message << " | FUNCTION: " << log_event.function_name
               << " | FILE: " << std::filesystem::path(log_event.file_name).filename()
               << " | LINE: " << log_event.line;
        return output.str();
    }

    auto errorFormatter(const LogEvent& log_event) -> std::string { return debugFormatter(log_event); }

    auto warningFormatter(const LogEvent& log_event) -> std::string { return infoFormatter(log_event); }

    auto infoFormatter(const LogEvent& log_event) -> std::string {
        std::stringstream output;
        tm tm_time{};
        std::time_t time = std::chrono::system_clock::to_time_t(log_event.time_point);
        gmtime_r(const_cast< const std::time_t* >(&time), &tm_time);
        output << std::put_time(const_cast< const tm* >(&tm_time), "%FT%T%Z") << " | " << std::left
               << std::setw(g_message_type_output_width) << log_event.message_type_string << " | "
               << log_event.module_name << " | "
               << "MESSAGE: " << log_event.message;
        return output.str();
    }

    auto fatalFormatter(const LogEvent& log_event) -> std::string { return debugFormatter(log_event); }
}  // End of unnamed namespace
