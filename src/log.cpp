#include "log.hpp"


#include <fstream>
#include <iostream>
#include <sstream>
#include <ctime>

#if defined (_WIN32) || defined(_WIN64)
#include <cerrno>
#endif

#include <thread>

using namespace tristan::log;

namespace{

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
    auto traceFormatter(const std::chrono::time_point<std::chrono::system_clock>& time_point, const std::string& message, [[maybe_unused]] const std::string& message_type,
                        [[maybe_unused]] const std::string& function_name,
                        [[maybe_unused]] const std::string& file_name, [[maybe_unused]] int line
    ) -> std::string;
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
    auto debugFormatter(const std::chrono::time_point<std::chrono::system_clock>& time_point, const std::string& message, [[maybe_unused]] const std::string& message_type,
                        [[maybe_unused]] const std::string& function_name,
                        [[maybe_unused]] const std::string& file_name, [[maybe_unused]] int line
    ) -> std::string;
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
    auto errorFormatter(const std::chrono::time_point<std::chrono::system_clock>& time_point, const std::string& message, [[maybe_unused]] const std::string& message_type,
                        [[maybe_unused]] const std::string& function_name,
                        [[maybe_unused]] const std::string& file_name, [[maybe_unused]] int line
    ) -> std::string;
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
    auto warningFormatter(const std::chrono::time_point<std::chrono::system_clock>& time_point, const std::string& message, [[maybe_unused]] const std::string& message_type,
                          [[maybe_unused]] const std::string& function_name,
                          [[maybe_unused]] const std::string& file_name, [[maybe_unused]] int line
    ) -> std::string;
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
    auto infoFormatter(const std::chrono::time_point<std::chrono::system_clock>& time_point, const std::string& message, [[maybe_unused]] const std::string& message_type,
                       [[maybe_unused]] const std::string& function_name,
                       [[maybe_unused]] const std::string& file_name, [[maybe_unused]] int line
    ) -> std::string;
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
    auto fatalFormatter(const std::chrono::time_point<std::chrono::system_clock>& time_point, const std::string& message, [[maybe_unused]] const std::string& message_type,
                        [[maybe_unused]] const std::string& function_name,
                        [[maybe_unused]] const std::string& file_name, [[maybe_unused]] int line
    ) -> std::string;

    /**
     * \brief Used to handle field width which is set by default formatter using std::setw call.
     */
    inline uint8_t g_message_type_output_width = 7;

} //End of unnamed namespace

Log::Log(){

    m_message_types.emplace_back("TRACE");
    m_message_types.emplace_back("DEBUG");
    m_message_types.emplace_back("ERROR");
    m_message_types.emplace_back("WARNING");
    m_message_types.emplace_back("INFO");
    m_message_types.emplace_back("FATAL");

    m_outputs.emplace_back(&std::cout);
    m_outputs.emplace_back(&std::cout);
    m_outputs.emplace_back(&std::cerr);
    m_outputs.emplace_back(&std::cout);
    m_outputs.emplace_back(&std::cout);
    m_outputs.emplace_back(&std::cerr);

    m_formatters.emplace_back(traceFormatter);
    m_formatters.emplace_back(debugFormatter);
    m_formatters.emplace_back(errorFormatter);
    m_formatters.emplace_back(warningFormatter);
    m_formatters.emplace_back(infoFormatter);
    m_formatters.emplace_back(fatalFormatter);

}

void Log::setMessageTypeOutput(MessageType message_type, const std::string& value){
    Log::instance().m_message_types.at(static_cast<size_t>(message_type)) = value;
    if (value.length() > g_message_type_output_width){
        g_message_type_output_width = value.length();
    }
}

void Log::setGlobalOutput(std::ostream* output_stream){
    auto& outputs = Log::instance().m_outputs;
    for (auto& output : outputs){
        if (output_stream == nullptr){
            output = std::monostate();
        }
        else{
            output = output_stream;
        }
    }
}

void Log::setGlobalOutput(const std::filesystem::path& file){
    auto& outputs = Log::instance().m_outputs;
    for (auto& output : outputs){
        output = file;
    }
}

void Log::setGlobalOutput(std::function<void(const std::string&)>&& output_func){
    for (auto& l_output : Log::instance().m_outputs){
        if (output_func == nullptr){
            l_output = std::monostate();
        }
        else{
            l_output = output_func;
        }
    }
}

void Log::setOutput(MessageType message_type, std::ostream* output_stream){
    if (output_stream == nullptr){
        Log::instance().m_outputs.at(static_cast<size_t>(message_type)) = std::monostate();
    }
    else{
        Log::instance().m_outputs.at(static_cast<size_t>(message_type)) = output_stream;
    }
}

void Log::setOutput(MessageType message_type, const std::filesystem::path& file){
    Log::instance().m_outputs.at(static_cast<size_t>(message_type)) = file;
}

void Log::setOutput(MessageType message_type, std::function<void(const std::string&)>&& output_func){
    if (output_func == nullptr){
        Log::instance().m_outputs.at(static_cast<size_t>(message_type)) = std::monostate();
    }
    else{
        Log::instance().m_outputs.at(static_cast<size_t>(message_type)) = output_func;
    }
}

void Log::setGlobalFormatter(
    std::function<std::string(const std::chrono::time_point<std::chrono::system_clock>&, const std::string&, const std::string&, const std::string&, const std::string&, int)>&& formatter){
    for (auto& l_formatter : Log::instance().m_formatters){
        l_formatter = formatter;
    }
}

void Log::setFormatter(MessageType message_type,
                       std::function<std::string(const std::chrono::time_point<std::chrono::system_clock>&, const std::string&, const std::string&, const std::string&, const std::string&,
                                                 int)>&& formatter){
    Log::instance().m_formatters.at(static_cast<size_t>(message_type)) = formatter;
}

void Log::write(const std::string& message, MessageType message_type, const std::string& function_name, const std::string& file, int line){

#if defined (LOG_DISABLE_TRACE)
    if (message_type == MessageType::Trace){
      return;
    }
#endif
#if defined (LOG_DISABLE_DEBUG)
    if (message_type == MessageType::Debug){
      return;
    }
#endif
#if defined (LOG_DISABLE_ERROR)
    if (message_type == MessageType::Error){
      return;
    }
#endif
#if defined (LOG_DISABLE_WARNING)
    if (message_type == MessageType::Warning){
      return;
    }
#endif
#if defined (LOG_DISABLE_INFO)
    if (message_type == MessageType::Info){
      return;
    }
#endif
#if defined (LOG_DISABLE_FATAL)
    if (message_type == MessageType::Fatal){
      return;
    }
#endif
    if (std::holds_alternative<std::monostate>(Log::instance().m_outputs.at(static_cast<size_t>(message_type)))){
        return;
    }
    try{
        Log::instance()._write(message, message_type, function_name, file, line);
    }
    catch (const std::fstream::failure& e){
        std::cerr << e.what() << ": " << e.code().message() << std::endl;
        //TODO: Decide if a simple report will be Ok, or we should exit the application with error code
    }

}

void Log::_write(const std::string& message, MessageType message_type, const std::string& function_name, const std::string& file, int line){
    auto time_stamp = std::chrono::system_clock::now();
    auto formatter = m_formatters.at(static_cast<size_t>(message_type));
    std::string msg = formatter(time_stamp, message, m_message_types.at(static_cast<size_t>(message_type)), function_name, file, line);
    std::visit([this, &msg](auto&& arg) -> void{
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::ostream*>){
            std::scoped_lock<std::mutex> lock(m_mutex);
            *arg << msg << std::endl;
        }
        else if constexpr (std::is_same_v<T, std::filesystem::path>){
            std::scoped_lock<std::mutex> lock(m_mutex);
            std::ofstream file(arg, std::ios::app);
            if (!file.is_open()){
                throw std::fstream::failure("Could not open Log file for writing - ", std::error_code(errno, std::system_category()));
            }
            file << msg << std::endl;
            file.close();
        }
        else if constexpr(std::is_same_v<T, std::function<void(const std::string&)>>){
            arg(msg);
        }
    }, m_outputs.at(static_cast<size_t>(message_type)));
}

auto Log::instance() -> Log&{

    static Log log;
    return log;
}

namespace{

    auto traceFormatter(const std::chrono::time_point<std::chrono::system_clock>& time_point, const std::string& message, const std::string& message_type, const std::string& function_name,
                        const std::string& file_name, int line
    ) -> std::string{
        std::stringstream output;
        output << std::this_thread::get_id() << " | " << time_point.time_since_epoch().count() << " | " << function_name << " | " << message;
        return output.str();
    }

    auto debugFormatter(const std::chrono::time_point<std::chrono::system_clock>& time_point, const std::string& message, const std::string& message_type, const std::string& function_name,
                        const std::string& file_name, int line
    ) -> std::string{
        std::stringstream output;
        tm tm_time{};
        std::time_t time = std::chrono::system_clock::to_time_t(time_point);
        localtime_r(const_cast<const std::time_t*>(&time), &tm_time);
        output << std::put_time(const_cast<const tm*>(&tm_time), "%FT%T") << " | " << std::left << std::setw(g_message_type_output_width) << message_type << " | " << message << " in function "
               << function_name << " of file " << std::filesystem::path(file_name).filename()
               << " at line " << line;
        return output.str();
    }

    auto errorFormatter(const std::chrono::time_point<std::chrono::system_clock>& time_point, const std::string& message, const std::string& message_type, const std::string& function_name,
                        const std::string& file_name, int line
    ) -> std::string{
        return infoFormatter(time_point, message, message_type, function_name, file_name, line);
    }

    auto warningFormatter(const std::chrono::time_point<std::chrono::system_clock>& time_point, const std::string& message, const std::string& message_type, const std::string& function_name,
                          const std::string& file_name, int line
    ) -> std::string{
        return infoFormatter(time_point, message, message_type, function_name, file_name, line);
    }

    auto infoFormatter(const std::chrono::time_point<std::chrono::system_clock>& time_point, const std::string& message, const std::string& message_type, const std::string& function_name,
                       const std::string& file_name, int line
    ) -> std::string{
        std::stringstream output;
        tm tm_time{};
        std::time_t time = std::chrono::system_clock::to_time_t(time_point);
        localtime_r(const_cast<const std::time_t*>(&time), &tm_time);
        output << std::put_time(const_cast<const tm*>(&tm_time), "%FT%T") << " | " << std::left << std::setw(g_message_type_output_width) << message_type << " | " << message;
        return output.str();
    }

    auto fatalFormatter(const std::chrono::time_point<std::chrono::system_clock>& time_point, const std::string& message, const std::string& message_type, const std::string& function_name,
                        const std::string& file_name, int line
    ) -> std::string{
        return debugFormatter(time_point, message, message_type, function_name, file_name, line);
    }
} //End of unnamed namespace
