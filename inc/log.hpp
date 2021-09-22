#ifndef LOG_LOG_HPP
#define LOG_LOG_HPP

#include <string>
#include <filesystem>
#include <mutex>
#include <unordered_map>
#include <variant>

namespace tristan::log
{
    enum class MessageType : uint8_t
    {
        Debug,
        Error,
        Info,
        Fatal,
        Trace,
        Warning
    };
    
    enum class OutputStream : uint8_t
    {
        Cerr,
        Cout
    };
    
    class Log
    {
    public:
        Log(const Log&) = delete;
        Log(Log&&) = delete;
        //OPERATORS
        Log& operator=(const Log&) = delete;
        Log& operator=(Log&&) = delete;
        static void createLogDirectory(const std::filesystem::path &directory);
        static void setGlobalOutput(OutputStream output_stream);
        static void setGlobalOutput(const std::filesystem::path &file);
        static void setOutput(MessageType message_type, OutputStream output_stream);
        static void setOutput(MessageType message_type, const std::filesystem::path &file);
        static void write(std::string_view message, MessageType message_type, std::string_view function_name = "", std::string_view file = "", int line = 0);
        ~Log() = default;
    
        static auto instance() -> Log&;
    private:
        Log();
    
        void _write(std::string_view message, MessageType message_type, std::string_view function_name, std::string_view file, std::string_view line);
    
        std::mutex m_mutex;
        std::unordered_map<MessageType, std::string> m_message_types;
        std::unordered_map<MessageType, std::variant<OutputStream, std::filesystem::path> > m_outputs;
    };

#define WriteDebug(message) Log::write(message, MessageType::Debug, __PRETTY_FUNCTION__, __FILE__, __LINE__)
#define WriteError(message) Log::write(message, MessageType::Error, __PRETTY_FUNCTION__, __FILE__, __LINE__)
#define WriteInfo(message) Log::write(message, MessageType::Info, __PRETTY_FUNCTION__, __FILE__, __LINE__)
#define WriteFatal(message) Log::write(message, MessageType::Fatal, __PRETTY_FUNCTION__, __FILE__, __LINE__)
#define WriteTrace(message) Log::write(message, MessageType::Trace, __PRETTY_FUNCTION__, __FILE__, __LINE__)
#define WriteWarning(message) Log::write(message, MessageType::Warning, __PRETTY_FUNCTION__, __FILE__, __LINE__)

} //End of tristan namespace

#endif //LOG_LOG_HPP
