#include "log.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#if defined (_WIN32) || defined(_WIN64)
#include <cerrno>
#endif

using namespace tristan::log;

namespace{
    
    /**
     * \internal
     * \brief Default formatter function for Trace messages.
     * \param time_point const std::chrono::time_point<std::chrono::system_clock>&
     * \param message const std::string& message
     * \param message_type const std::string& message
     * \param function_name const std::string& message
     * \param file_name const std::string& message
     * \param line int
     * \return std::string which stores formatted message which will be sent to output
     */
    auto traceFormatter(const std::chrono::time_point<std::chrono::system_clock>& time_point, const std::string& message,
                        [[maybe_unused]] const std::string& message_type,
                        [[maybe_unused]] const std::string& function_name,
                        [[maybe_unused]] const std::string& file_name,
                        [[maybe_unused]] int line) -> std::string;
    /**
     * \internal
     * \brief Default formatter function for Debug messages.
     * \param time_point const std::chrono::time_point<std::chrono::system_clock>&
     * \param message const std::string& message
     * \param message_type const std::string& message
     * \param function_name const std::string& message
     * \param file_name const std::string& message
     * \param line int
     * \return std::string which stores formatted message which will be sent to output
     */
    auto debugFormatter(const std::chrono::time_point<std::chrono::system_clock>& time_point, const std::string& message,
                        [[maybe_unused]] const std::string& message_type,
                        [[maybe_unused]] const std::string& function_name,
                        [[maybe_unused]] const std::string& file_name,
                        [[maybe_unused]] int line) -> std::string;
    /**
     * \internal
     * \brief Default formatter function for Error messages.
     * \param time_point const std::chrono::time_point<std::chrono::system_clock>&
     * \param message const std::string& message
     * \param message_type const std::string& message
     * \param function_name const std::string& message
     * \param file_name const std::string& message
     * \param line int
     * \return std::string which stores formatted message which will be sent to output
     */
    auto errorFormatter(const std::chrono::time_point<std::chrono::system_clock>& time_point, const std::string& message,
                        [[maybe_unused]] const std::string& message_type,
                        [[maybe_unused]] const std::string& function_name,
                        [[maybe_unused]] const std::string& file_name,
                        [[maybe_unused]] int line) -> std::string;
    /**
     * \internal
     * \brief Default formatter function for Warning messages.
     * \param time_point const std::chrono::time_point<std::chrono::system_clock>&
     * \param message const std::string& message
     * \param message_type const std::string& message
     * \param function_name const std::string& message
     * \param file_name const std::string& message
     * \param line int
     * \return std::string which stores formatted message which will be sent to output
     */
    auto warningFormatter(const std::chrono::time_point<std::chrono::system_clock>& time_point, const std::string& message,
                          [[maybe_unused]] const std::string& message_type,
                          [[maybe_unused]] const std::string& function_name,
                          [[maybe_unused]] const std::string& file_name,
                          [[maybe_unused]] int line) -> std::string;
    /**
     * \internal
     * \brief Default formatter function for Info messages.
     * \param time_point const std::chrono::time_point<std::chrono::system_clock>&
     * \param message const std::string& message
     * \param message_type const std::string& message
     * \param function_name const std::string& message
     * \param file_name const std::string& message
     * \param line int
     * \return std::string which stores formatted message which will be sent to output
     */
    auto infoFormatter(const std::chrono::time_point<std::chrono::system_clock>& time_point, const std::string& message,
                       [[maybe_unused]] const std::string& message_type,
                       [[maybe_unused]] const std::string& function_name,
                       [[maybe_unused]] const std::string& file_name,
                       [[maybe_unused]] int line) -> std::string;
    /**
     * \internal
     * \brief Default formatter function for Fatal messages.
     * \param time_point const std::chrono::time_point<std::chrono::system_clock>&
     * \param message const std::string& message
     * \param message_type const std::string& message
     * \param function_name const std::string& message
     * \param file_name const std::string& message
     * \param line int
     * \return std::string which stores formatted message which will be sent to output
     */
    auto fatalFormatter(const std::chrono::time_point<std::chrono::system_clock>& time_point, const std::string& message,
                        [[maybe_unused]] const std::string& message_type,
                        [[maybe_unused]] const std::string& function_name,
                        [[maybe_unused]] const std::string& file_name,
                        [[maybe_unused]] int line) -> std::string;
    
    /**
     * \brief Used to handle field width which is set by default formatter using std::setw call.
     */
    inline uint8_t g_message_type_output_width = 7;
    
} //End of unnamed namespace

Log::Log() : m_log_level(MessageType::Trace){
  
  m_message_types.emplace(MessageType::Trace, "TRACE");
  m_message_types.emplace(MessageType::Debug, "DEBUG");
  m_message_types.emplace(MessageType::Error, "ERROR");
  m_message_types.emplace(MessageType::Warning, "WARNING");
  m_message_types.emplace(MessageType::Info, "INFO");
  m_message_types.emplace(MessageType::Fatal, "FATAL");
  
  m_outputs.emplace(MessageType::Trace, &std::cout);
  m_outputs.emplace(MessageType::Debug, &std::cout);
  m_outputs.emplace(MessageType::Error, &std::cerr);
  m_outputs.emplace(MessageType::Warning, &std::cout);
  m_outputs.emplace(MessageType::Info, &std::cout);
  m_outputs.emplace(MessageType::Fatal, &std::cerr);
  
  m_formatters.emplace(MessageType::Trace, traceFormatter);
  m_formatters.emplace(MessageType::Debug, debugFormatter);
  m_formatters.emplace(MessageType::Error, errorFormatter);
  m_formatters.emplace(MessageType::Warning, warningFormatter);
  m_formatters.emplace(MessageType::Info, infoFormatter);
  m_formatters.emplace(MessageType::Fatal, fatalFormatter);
  
}

void Log::setLogLevel(MessageType message_type){
  Log::instance().m_log_level = message_type;
}

void Log::setMessageTypeOutput(MessageType message_type, const std::string& value){
  Log::instance().m_message_types.at(message_type) = value;
  if (value.length() > g_message_type_output_width){
    g_message_type_output_width = value.length();
  }
}

void Log::setGlobalOutput(std::ostream* output_stream){
  auto& outputs = Log::instance().m_outputs;
  for (auto& output : outputs){
    output.second = output_stream;
  }
}

void Log::setGlobalOutput(const std::filesystem::path& file){
  auto& outputs = Log::instance().m_outputs;
  for (auto& output : outputs){
    output.second = file;
  }
}

void Log::setGlobalOutput(std::function<void(const std::string&)>&& output_func){
  for (auto& l_output : Log::instance().m_outputs){
    l_output.second = output_func;
  }
}

void Log::setOutput(MessageType message_type, std::ostream* output_stream){
  Log::instance().m_outputs.at(message_type) = output_stream;
}

void Log::setOutput(MessageType message_type, const std::filesystem::path& file){
  Log::instance().m_outputs.at(message_type) = file;
}

void Log::setOutput(MessageType message_type, std::function<void(const std::string&)>&& output_func){
  Log::instance().m_outputs.at(message_type) = output_func;
}

void Log::setGlobalFormatter(std::function<std::string(const std::chrono::time_point<std::chrono::system_clock>&, const std::string&, const std::string&, const std::string&, const std::string&,
                                                       int)>&& formatter){
  for (auto& l_formatter : Log::instance().m_formatters){
    l_formatter.second = formatter;
  }
}

void Log::setFormatter(MessageType message_type,
                       std::function<std::string(const std::chrono::time_point<std::chrono::system_clock>&, const std::string&, const std::string&, const std::string&, const std::string&,
                                                 int)>&& formatter){
  Log::instance().m_formatters.at(message_type) = formatter;
}

void Log::write(const std::string& message, MessageType message_type, const std::string& function_name, const std::string& file,
                int line){
  
  try{
    Log::instance()._write(message, message_type, function_name, file, line);
  }
  catch (const std::fstream::failure& e){
    std::cerr << e.what() << ": " << e.code().message() << std::endl;
    //TODO: Decide if a simple report will be Ok, or we should exit the application with error code
  }
  
}

void Log::_write(const std::string& message, MessageType message_type, const std::string& function_name, const std::string& file,
                 int line){
  if (message_type < m_log_level){
    return;
  }
  auto time_stamp = std::chrono::system_clock::now();
  auto formatter = m_formatters.at(message_type);
  std::string msg = formatter(time_stamp, message, m_message_types.at(message_type), function_name, file, line);
  std::visit(
          [this, &msg](auto&& arg) -> void{
              using T = std::decay_t<decltype(arg)>;
              if constexpr (std::is_same_v<T, std::ostream*>){
                std::scoped_lock<std::mutex> lock(m_mutex);
                if (arg != nullptr){
                  *arg << msg << std::endl;
                }
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
              else{
                arg(msg);
              }
          }, m_outputs.at(message_type)
  );
}

auto Log::instance() -> Log&{
  
  static Log log;
  return log;
}


namespace{
    auto traceFormatter(const std::chrono::time_point<std::chrono::system_clock>& time_point, const std::string& message, const std::string& message_type, const std::string& function_name,
                        const std::string& file_name, int line) -> std::string{
      tm tm_time{};
      std::time_t time_t_time = std::chrono::system_clock::to_time_t(time_point);
      localtime_s(&tm_time, &time_t_time);
      std::stringstream output;
      output << std::put_time(&tm_time, "%T") << " | " << std::left << std::setw(g_message_type_output_width) << message_type << " | " << message << " in function " << function_name;
      return output.str();
    }
    
    auto debugFormatter(const std::chrono::time_point<std::chrono::system_clock>& time_point, const std::string& message, const std::string& message_type, const std::string& function_name,
                        const std::string& file_name, int line) -> std::string{
      tm tm_time{};
      std::time_t time_t_time = std::chrono::system_clock::to_time_t(time_point);
      localtime_s(&tm_time, &time_t_time);
      std::stringstream output;
      output << std::put_time(&tm_time, "%T") << " | " << std::left << std::setw(g_message_type_output_width) << message_type << " | " << message << " in function " << function_name << " of file " << file_name
      << " at line " << line;
      return output.str();
    }
    
    auto errorFormatter(const std::chrono::time_point<std::chrono::system_clock>& time_point, const std::string& message, const std::string& message_type, const std::string& function_name,
                        const std::string& file_name, int line) -> std::string{
      return infoFormatter(time_point, message, message_type, function_name, file_name, line);
    }
    
    auto warningFormatter(const std::chrono::time_point<std::chrono::system_clock>& time_point, const std::string& message, const std::string& message_type, const std::string& function_name,
                          const std::string& file_name, int line) -> std::string{
      return infoFormatter(time_point, message, message_type, function_name, file_name, line);
    }
    
    auto infoFormatter(const std::chrono::time_point<std::chrono::system_clock>& time_point, const std::string& message, const std::string& message_type, const std::string& function_name,
                       const std::string& file_name, int line) -> std::string{
      tm tm_time{};
      std::time_t time_t_time = std::chrono::system_clock::to_time_t(time_point);
      localtime_s(&tm_time, &time_t_time);
      std::stringstream output;
      output << std::put_time(&tm_time, "%T") << " | " << std::left << std::setw(g_message_type_output_width) << message_type << " | " << message;
      return output.str();
    }
    
    auto fatalFormatter(const std::chrono::time_point<std::chrono::system_clock>& time_point, const std::string& message, const std::string& message_type, const std::string& function_name,
                        const std::string& file_name, int line) -> std::string{
      return debugFormatter(time_point, message, message_type, function_name, file_name, line);
    }
} //End of unnamed namespace
