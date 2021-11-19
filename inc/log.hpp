#ifndef LOG_HPP
#define LOG_HPP

#include <string>
#include <filesystem>
#include <mutex>
#include <unordered_map>
#include <variant>
#include <ostream>
#include <functional>
#include <chrono>

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

namespace tristan::log{
    
    /**
     * \enum MessageType
     * \brief List of supported message types
     */
    enum class MessageType : uint8_t{
        Trace,
        Debug,
        Error,
        Warning,
        Info,
        Fatal
    };
    
    /**
     * \class Log
     * \brief Implements logging logic of licensing module and provides API for customization.
     * Class is implemented as a singleton.
     *
     * The following customizations are possible:
     * \par Output
     * \nOutput may be set globally, that is for all message types, or for each message type separately using the setGlobalOutput and setOutput sets of functions respectively. Output may be one of
     * the following:
     * \format std::ostream*
     * \format const std::filesystem::path&
     * \format std::function\<void(const std::string&)\>
     * \format std::shared_ptr<Class> and a pointer to a member function which accepts const std::string& as a parameter. This should be considered as preferable way of using member functions since
     * the validity of std::shared_prt is checked before function invocation.
     * \format Pointer to an object and a pointer to a member function which accepts const std::string& as a parameter \note This function should be used with caution as the validity of pointer is
     * not checked before function invocation.
     * \attention Log write function is thread safe when output is set either to std::ostream or to std::filesystem::path and not thread safe in case of user defined callbacks. That is it is a user
     * obligation to handle multithreaded calls of provided callback function.
     * \nOutput may be limited by log level using setLogLevel function.
     * \nOutput may be disabled for any of message types by passing the [nullptr] value to setOutput function.
     * \nBy default the log level is set to Trace which means that all types of messages will be sent to output and the output itself is set to std::cerr for Error and Fatal and std::cout for other
     * message types.
     * \par Formatting
     * \nDefault formatting is set to the following:
     * \format Trace -   hh:mm:ss | TRACE | [message] in [function_name]
     * \format Debug -   hh:mm:ss | DEBUG | [message] in [function_name] of [file_name] at line [line]
     * \format Error -   hh:mm:ss | ERROR | [message]
     * \format Warning - hh:mm:ss | TRACE | [message]
     * \format Info -    hh:mm:ss | TRACE | [message]
     * \format Fatal -   hh:mm:ss | TRACE | [message] in [function_name] of [file_name] at line [line]
     * \nUser may provide custom formatting function which may be set globally, that is for all message types, or for each message type separately using the setGlobalOutput and setOutput sets of
     * functions respectively. Output may be one of
     * the following:
     * \format std::function\<std::string(const std::chrono::time_point\<std::chrono::system_clock\>&, const std::string&, const std::string&, const std::string&, const std::string&, int)\>
     * \format std::shared_ptr\<Class\> and a pointer to a member function which accepts const std::chrono::time_point\<std::chrono::system_clock\>&, const std::string&, const std::string&, const
     * std::string&, const std::string& and int as parameters. This should be considered as preferable way of using member functions since the validity of std::shared_prt is checked before function
     * invocation.
     * \format Pointer to an object and a pointer to a member function which accepts const std::chrono::time_point\<std::chrono::system_clock\>&, const std::string&, const std::string&, const
     * std::string&, const std::string& and int as parameters. \note This function should be used with caution as the validity of pointer is not checked before function invocation.
     */
    class Log{
    public:
        Log(const Log&) = delete;
        Log(Log&&) = delete;
        //OPERATORS
        Log& operator=(const Log&) = delete;
        Log& operator=(Log&&) = delete;
        
        /**
         * \brief Sets log level according to provided message type.
         *
         * This function sets level of logging taking into account the order of message types defined in MessageTypes enum.
         * The level is set using less comparison operator. That is message is sent to output only if message type of the latter is equal or greater to set log level.
         * For example, if log level is set to MessageType::Info, only messages with message type Info and Fatal are sent to output.
         *
         * \param message_type MessageType
         */
        static void setLogLevel(MessageType message_type);
        
        /**
         * \brief Sets the string which will be sent to output.
         * \param message_type MessageType
         * \param value const std::string&
         */
        static void setMessageTypeOutput(MessageType message_type, const std::string& value);
        
        /**
         * \brief Sets output for all message types.
         * \param output_stream std::ostream*
         */
        static void setGlobalOutput(std::ostream* output_stream);
        
        /**
         * \overload
         * \brief Sets output for all message types.
         * \param file const std::filesystem::path&
         */
        static void setGlobalOutput(const std::filesystem::path& file);
        
        /**
         * \overload
         * \brief Sets output for all message types.
         * \param output_func std::function\<void(const std::string&)\>&&
         */
        static void setGlobalOutput(std::function<void(const std::string&)>&& output_func);
        
        /**
         * \overload
         * \brief Sets output for all message types.
         * \tparam Object class which implements the function pointer passed as a second parameter
         * \param object std::shared_ptr\<Object\>
         * \param functor void (Object::*functor)(const std::string&)
         */
        template<class Object>
        static void setGlobalOutput(std::weak_ptr<Object> object, void (Object::*functor)(const std::string&));
        
        /**
         * \overload
         * \brief Sets output for all message types.
         * \tparam Object class which implements the function pointer passed as a second parameter
         * \param object Object*
         * \param functor void (Object::*functor)(const std::string&)
         */
        template<class Object>
        static void setGlobalOutput(Object* object, void (Object::*functor)(const std::string&));
        
        /**
         * \brief Sets output for specified message type.
         * \param message_type MessageType
         * \param output_stream std::ostream*
         */
        static void setOutput(MessageType message_type, std::ostream* output_stream);
    
        /**
         * \overload
         * \brief Sets output for specified message type.
         * \param message_type MessageType
         * \param output_stream const std::filesystem::path&
         */
        
        static void setOutput(MessageType message_type, const std::filesystem::path& file);
        /**
         * \overload
         * \brief Sets output for specified message type.
         * \param message_type MessageType
         * \param output_stream std::function\<void(const std::string&)\>&&
         */
        static void setOutput(MessageType message_type, std::function<void(const std::string&)>&& output_func);
        
        /**
         * \overload
         * \brief Sets output for specified message type.
         * \tparam Object class which implements the function pointer passed as a second parameter
         * \param message_type MessageType
         * \param object std::shared_ptr\<Object\>
         * \param functor void (Object::*functor)(const std::string&)
         */
        template<class Object>
        static void setOutput(MessageType message_type, std::weak_ptr<Object> object, void (Object::*functor)(const std::string&));
    
        /**
         * \overload
         * \brief Sets output for specified message type.
         * \tparam Object class which implements the function pointer passed as a second parameter
         * \param message_type MessageType
         * \param object Object*
         * \param functor void (Object::*functor)(const std::string&)
         */
        template<class Object>
        static void setOutput(MessageType message_type, Object* object, void (Object::*functor)(const std::string&));
        
        /**
         * \brief Sets formatter for all message types.
         * \param formatter std::function\<std::string(const std::chrono::time_point\<std::chrono::system_clock\>&, const std::string&, const std::string&, const std::string&, const std::string&,
         * int)\>&&
         */
        static void setGlobalFormatter(std::function<std::string(const std::chrono::time_point<std::chrono::system_clock>&, const std::string&, const std::string&, const std::string&,
                                                                 const std::string&, int)>&& formatter);
        
        /**
         * \overload
         * \brief Sets formatter for all message types.
         * \tparam Object class which implements the function pointer passed as a second parameter
         * \param object std::shared_ptr\<Object\>
         * \param functor void (Object::*functor)(const std::chrono::time_point\<std::chrono::system_clock\>&, const std::string&, const std::string&, const std::string&, const std::string&, int)
         */
        template<class Object>
        static void setGlobalFormatter(std::weak_ptr<Object> object,
                                       void (Object::*functor)(const std::chrono::time_point<std::chrono::system_clock>&, const std::string&, const std::string&, const std::string&,
                                                               const std::string&, int));
    
        /**
         * \overload
         * \brief Sets formatter for all message types.
         * \tparam Object class which implements the function pointer passed as a second parameter
         * \param object Object*
         * \param functor void (Object::*functor)(const std::chrono::time_point\<std::chrono::system_clock\>&, const std::string&, const std::string&, const std::string&, const std::string&, int)
         */
        template<class Object>
        static void setGlobalFormatter(Object* object,
                                       void (Object::*functor)(const std::chrono::time_point<std::chrono::system_clock>&, const std::string&, const std::string&, const std::string&,
                                                               const std::string&, int));
    
        /**
         * \brief Sets formatter for specified message type.
         * \param formatter std::function\std::string(const std::chrono::time_point\<std::chrono::system_clock\>&, const std::string&, const std::string&, const std::string&, const std::string&, int)
         * \>&&
         */
        static void setFormatter(MessageType message_type,
                                 std::function<std::string(const std::chrono::time_point<std::chrono::system_clock>&, const std::string&, const std::string&, const std::string&,
                                                           const std::string&, int)>&& formatter);
    
        /**
         * \overload
         * \brief Sets formatter for specified message type.
         * \tparam Object class which implements the function pointer passed as a second parameter
         * \param object std::shared_ptr\<Object\>
         * \param functor void (Object::*functor)(const std::chrono::time_point\<std::chrono::system_clock\>&, const std::string&, const std::string&, const std::string&, const std::string&, int)
         */
        template<class Object>
        static void setFormatter(MessageType message_type, std::weak_ptr<Object> object,
                                 void (Object::*functor)(const std::chrono::time_point<std::chrono::system_clock>&, const std::string&, const std::string&, const std::string&, const std::string&,
                                                         int));
    
        /**
         * \overload
         * \brief Sets formatter for specified message type.
         * \tparam Object class which implements the function pointer passed as a second parameter
         * \param object Object*
         * \param functor void (Object::*functor)(const std::chrono::time_point\<std::chrono::system_clock\>&, const std::string&, const std::string&, const std::string&, const std::string&, int)
         */
        template<class Object>
        static void setFormatter(MessageType message_type, Object* object,
                                 void (Object::*functor)(const std::chrono::time_point<std::chrono::system_clock>&, const std::string&, const std::string&, const std::string&, const std::string&,
                                                         int));
        
        /**
         * \brief Writes log message of preset format to preset output.
         * \param message const std::string&
         * \param message_type MessageType
         * \param function_name const std::string&
         * \param file const std::string&
         * \param line int
         */
        static void write(const std::string& message, MessageType message_type, const std::string& function_name, const std::string& file, int line);
        /**
         * \brief Destructor
         */
        ~Log() = default;
    
    private:
        Log();
        static auto instance() -> Log&;
        
        void _write(const std::string& message, MessageType message_type, const std::string& function_name, const std::string& file, int line);
        
        /**
         * \internal
         * \brief Mutex to handle multithreaded output if output is set to std::ostream or std::filesystem::path and thus the std::ofstream is used.
         */
        std::mutex m_mutex;
        
        /**
         * \internal
         * \brief Stores string representations of message types.
         */
        std::unordered_map<MessageType, std::string> m_message_types;
    
        /**
         * \internal
         * \brief Stores output for each message type.
         */
        std::unordered_map<MessageType, std::variant<std::ostream*, std::filesystem::path, std::function<void(const std::string&)>>> m_outputs;
        
        /**
         * \internal
         * \brief Stores formatter functions for each message type.
         */
        std::unordered_map<MessageType, std::function<std::string(const std::chrono::time_point<std::chrono::system_clock>&, const std::string&, const std::string&, const std::string&,
                                                                  const std::string&,
                                                                  int)> > m_formatters;
        
        /**
         * \internal
         * \brief Used to store desired log level.
         */
        MessageType m_log_level;
    };
    
    template<class Object>
    void Log::setGlobalOutput(std::weak_ptr<Object> object, void (Object::*functor)(const std::string&)){
      for (auto& output : Log::instance().m_outputs){
        output.second = [object, functor](const std::string& message){
            if (auto l_object = object.lock()){
              std::invoke(functor, l_object, message);
            }
        };
      }
    }
    
    template<class Object>
    void Log::setGlobalOutput(Object* object, void (Object::*functor)(const std::string&)){
      for (auto& output : Log::instance().m_outputs){
        output.second = [object, functor](const std::string& message){
            std::invoke(functor, object, message);
        };
      }
    }
    
    template<class Object>
    void Log::setOutput(MessageType message_type, std::weak_ptr<Object> object, void (Object::*functor)(const std::string&)){
      Log::instance().m_outputs.at(message_type) = [object, functor](const std::string& message){
          if (auto l_object = object.lock()){
            std::invoke(functor, l_object, message);
          }
      };
    }
    
    template<class Object>
    void Log::setOutput(MessageType message_type, Object* object, void (Object::*functor)(const std::string&)){
      Log::instance().m_outputs.at(message_type) = [object, functor](const std::string& message){
          std::invoke(functor, object, message);
      };
    }
    
    template<class Object>
    void Log::setGlobalFormatter(std::weak_ptr<Object> object,
                                 void (Object::*functor)(const std::chrono::time_point<std::chrono::system_clock>&, const std::string&, const std::string&, const std::string&, const std::string&,
                                                         int)){
      for (auto& formatter : Log::instance().m_formatters){
        formatter.second = [object, functor](const std::chrono::time_point<std::chrono::system_clock>& time_point,
                                             const std::string& message,
                                             const std::string& message_type,
                                             const std::string& function_name,
                                             const std::string& file_name,
                                             int line){
            if (auto l_object = object.lock()){
              std::invoke(functor, l_object, time_point, message, message_type, function_name, file_name, line);
            }
        };
      }
    }
    
    template<class Object>
    void Log::setGlobalFormatter(Object* object,
                                 void (Object::*functor)(const std::chrono::time_point<std::chrono::system_clock>&, const std::string&, const std::string&, const std::string&, const std::string&,
                                                         int)){
      for (auto& formatter : Log::instance().m_formatters){
        formatter.second = [object, functor](const std::chrono::time_point<std::chrono::system_clock>& time_point,
                                             const std::string& message,
                                             const std::string& message_type,
                                             const std::string& function_name,
                                             const std::string& file_name,
                                             int line){
            std::invoke(functor, object, time_point, message, message_type, function_name, file_name, line);
        };
      }
    }
    
    template<class Object>
    void Log::setFormatter(MessageType message_type, std::weak_ptr<Object> object,
                           void (Object::*functor)(const std::chrono::time_point<std::chrono::system_clock>&, const std::string&, const std::string&, const std::string&, const std::string&, int)){
      Log::instance().m_formatters.at(message_type) = [object, functor](const std::chrono::time_point<std::chrono::system_clock>& time_point,
                                                                        const std::string& message,
                                                                        const std::string& message_type,
                                                                        const std::string& function_name,
                                                                        const std::string& file_name,
                                                                        int line){
          if (auto l_object = object.lock()){
            std::invoke(functor, l_object, time_point, message, message_type, function_name, file_name, line);
          }
      };
    }
    
    template<class Object>
    void Log::setFormatter(MessageType message_type, Object* object,
                           void (Object::*functor)(const std::chrono::time_point<std::chrono::system_clock>&, const std::string&, const std::string&, const std::string&, const std::string&, int)){
      Log::instance().m_formatters.at(message_type) = [object, functor](const std::chrono::time_point<std::chrono::system_clock>& time_point,
                                                                        const std::string& message,
                                                                        const std::string& message_type,
                                                                        const std::string& function_name,
                                                                        const std::string& file_name,
                                                                        int line){
          std::invoke(functor, object, time_point, message, message_type, function_name, file_name, line);
      };
    }

/**
 * \def FLTrace(message)
 * Convenience macro to output Trace message.
 * \param message const std::string&
 */
#define FLTrace(message) FlexLicensing::Logging::Log::write(message, FlexLicensing::Logging::MessageType::Trace, __PRETTY_FUNCTION__, __FILE__, __LINE__)
/**
 * \def FLDebug(message)
 * Convenience macro to output Debug message.
 * \param message const std::string&
 */
#define FLDebug(message) FlexLicensing::Logging::Log::write(message, FlexLicensing::Logging::MessageType::Debug, __PRETTY_FUNCTION__, __FILE__, __LINE__)
/**
 * \def FLError(message)
 * Convenience macro to output Error message.
 * \param message const std::string&
 */
#define FLError(message) FlexLicensing::Logging::Log::write(message, FlexLicensing::Logging::MessageType::Error, __PRETTY_FUNCTION__, __FILE__, __LINE__)
/**
 * \def FLWarning(message)
 * Convenience macro to output Warning message.
 * \param message const std::string&
 */
#define FLWarning(message) FlexLicensing::Logging::Log::write(message, FlexLicensing::Logging::MessageType::Warning, __PRETTY_FUNCTION__, __FILE__, __LINE__)
/**
 * \def FLInfo(message)
 * Convenience macro to output Info message.
 * \param message const std::string&
 */
#define FLInfo(message) FlexLicensing::Logging::Log::write(message, FlexLicensing::Logging::MessageType::Info, __PRETTY_FUNCTION__, __FILE__, __LINE__)
/**
 * \def FLFatal(message)
 * Convenience macro to output Fatal message.
 * \param message const std::string&
 */
#define FLFatal(message) FlexLicensing::Logging::Log::write(message, FlexLicensing::Logging::MessageType::Fatal, __PRETTY_FUNCTION__, __FILE__, __LINE__)

} //End of FlexLicensing namespace

#endif //LOG_HPP

