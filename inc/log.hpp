#ifndef LOG_HPP
#define LOG_HPP

#include <chrono>
#include <filesystem>
#include <functional>
#include <mutex>
#include <ostream>
#include <string>
#include <thread>
#include <utility>
#include <variant>
#include <vector>

#ifdef _MSC_VER
  #define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

namespace tristan::log {

    /**
     * \enum MessageType
     * \brief List of supported message types
     */
    enum class MessageType : uint8_t {
        Trace,
        Debug,
        Error,
        Warning,
        Info,
        Fatal
    };

    struct LogEvent;

    /**
     * \class Log
     * \brief Implements logging logic and provides API for
     * customization.
     *
     * The following customizations are possible:
     * \par Output
     * \n Output may be set globally, that is for all message types, or for each
     * message type separately using the setGlobalOutput and setOutput sets of
     * functions respectively. Output may be one of the following:
     * \li std::ostream*
     * \li const std::filesystem::path&
     * \li std::function\<void(const std::string&)\>
     * \li std::weak_ptr<Class> and a pointer to a member function which accepts
     * const std::string& as a parameter. This should be considered as preferable
     * way of using member functions since the validity of std::weak_prt is checked
     * before function invocation.
     * \li Pointer to an object and a pointer to a
     * member function which accepts const std::string& as a parameter. This
     * function should be used with caution as the validity of pointer is not
     * checked before function invocation.
     *
     * \n Output may be disabled for each message type using the following self
     * explanatory preprocessor directives:
     * \li LOG_DISABLE_TRACE
     * \li LOG_DISABLE_DEBUG
     * \li LOG_DISABLE_ERROR
     * \li LOG_DISABLE_WARNING
     * \li LOG_DISABLE_INFO
     * \li LOG_DISABLE_FATAL
     *
     * \n Additionally output may be disabled for any of message types on runtime by
     * passing the [nullptr] value to setOutput function.
     * \par Formatting
     * \n Default formatting is set to the following:
     * \li Trace   - thread_id | time_since_epoch | TRACE | [module_name] | [function_name] | [message]
     * \li Debug   - hh:mm:ss | DEBUG   | [module_name] | MESSAGE: [message] FUNCTION: [function_name] FILE: [file_name] LINE: [line]
     * \li Error   - hh:mm:ss | ERROR   | [module_name] | MESSAGE: [message]
     * \li Warning - hh:mm:ss | WARNING | [module_name] | MESSAGE: [message]
     * \li Info    - hh:mm:ss | INFO    | [module_name] | MESSAGE: [message]
     * \li Fatal   - hh:mm:ss | FATAL   | [module_name] | MESSAGE: [message] FUNCTION: [function_name] FILE: [file_name] LINE: [line]
     *
     * \n User may provide custom formatting function which may be set globally,
     * that is for all message types, or for each message type separately using the
     * setGlobalFormatter and setFormatter sets of functions respectively. Formatter
     * may be one of the following:
     * \li std::function\<std::string(LogEvent&&)\>
     * \li std::weak_ptr\<Class\> and a pointer to a member function which accepts LogEvent&& as
     * parameter. This should be considered as preferable way of using member
     * functions since the validity of std::shared_prt is checked before function
     * invocation.
     * \li Pointer to an object and a pointer to a member function which accepts
     * LogEvent&& as parameter. This function should be used with caution as the validity
     * of pointer is not checked before function invocation.
     *
     * \attention Write function is thread safe when output is set either to
     * std::ostream or to std::filesystem::path and not thread safe in case of user
     * defined callbacks. That is it is a user obligation to handle multi-threaded
     * calls of provided callback function.
     */
    class Log {
    public:
        /**
         * \private
         */
        Log(const Log&) = delete;
        /**
         * \private
         */
        Log(Log&&) = delete;
        /**
         * \private
         */
        Log& operator=(const Log&) = delete;
        /**
         * \private
         */
        Log& operator=(Log&&) = delete;

        /**
         * \brief Sets the string representation of MessageType.
         *
         * Default values are as follows:
         * \li TRACE
         * \li DEBUG
         * \li ERROR
         * \li WARNING
         * \li INFO
         * \li FATAL
         *
         * \param message_type MessageType
         * \param value const std::string&
         */
        void setMessageTypeOutput(MessageType message_type, const std::string& value);

        /**
         * \brief Sets output for all message types.
         * \param output_stream std::ostream*
         */
        void setGlobalOutput(std::ostream* output_stream);

        /**
         * \overload
         * \brief Sets output for all message types.
         * \param file const std::filesystem::path&
         */
        void setGlobalOutput(const std::filesystem::path& file);

        /**
         * \overload
         * \brief Sets output for all message types.
         * \param output_func std::function\<void(const std::string&)\>&&
         */
        void setGlobalOutput(std::function< void(const std::string&) >&& output_func);

        /**
         * \overload
         * \brief Sets output for all message types.
         * \tparam Object class which implements the function pointer passed as a
         * second parameter \param object std::shared_ptr\<Object\> \param functor
         * void (Object::*functor)(const std::string&)
         */
        template < class Object >
        void setGlobalOutput(std::weak_ptr< Object > object, void (Object::*functor)(const std::string&));

        /**
         * \overload
         * \brief Sets output for all message types.
         * \tparam Object class which implements the function pointer passed as a
         * second parameter \param object Object* \param functor void
         * (Object::*functor)(const std::string&)
         */
        template < class Object >
        void setGlobalOutput(Object* object, void (Object::*functor)(const std::string&));

        /**
         * \brief Sets output for specified message type.
         * \param message_type MessageType
         * \param output_stream std::ostream*
         */
        void setOutput(MessageType message_type, std::ostream* output_stream);

        /**
         * \overload
         * \brief Sets output for specified message type.
         * \param message_type MessageType
         * \param file const std::filesystem::path&
         */

        void setOutput(MessageType message_type, const std::filesystem::path& file);
        /**
         * \overload
         * \brief Sets output for specified message type.
         * \param message_type MessageType
         * \param output_func std::function\<void(const std::string&)\>&&
         */
        void setOutput(MessageType message_type, std::function< void(const std::string&) >&& output_func);

        /**
         * \overload
         * \brief Sets output for specified message type.
         * \tparam Object class which implements the function pointer passed as a second parameter
         * \param message_type MessageType
         * \param object std::shared_ptr\<Object\>
         * \param functor void (Object::*functor)(const std::string&)
         */
        template < class Object >
        void setOutput(MessageType message_type,
                       std::weak_ptr< Object > object,
                       void (Object::*functor)(const std::string&));

        /**
         * \overload
         * \brief Sets output for specified message type.
         * \tparam Object class which implements the function pointer passed as a second parameter
         * \param message_type MessageType \param object Object*
         * \param functor void (Object::*functor)(const std::string&)
         */
        template < class Object >
        void setOutput(MessageType message_type, Object* object, void (Object::*functor)(const std::string&));

        /**
         * \brief Sets formatter for all message types.
         * \param formatter std::function\<std::string(LogEvent&&)\>
         */
        void setGlobalFormatter(std::function< std::string(LogEvent&& log_event) >&& formatter);

        /**
         * \overload
         * \brief Sets formatter for all message types.
         * \tparam Object class which implements the function pointer passed as a second parameter
         * \param object std::shared_ptr\<Object\>
         * \param functor void (Object::*functor)(LogEvent&&)
         */
        template < class Object >
        void setGlobalFormatter(std::weak_ptr< Object > object,
                                void (Object::*functor)(LogEvent&& log_event));

        /**
         * \overload
         * \brief Sets formatter for all message types.
         * \tparam Object class which implements the function pointer passed as a second parameter
         * \param object Object*
         * \param functor void (Object::*functor)(LogEvent&&)
         */
        template < class Object >
        void setGlobalFormatter(Object* object, void (Object::*functor)(LogEvent&& log_event));

        /**
         * \brief Sets formatter for specified message type.
         * \param message_type MessageType
         * \param formatter std::function\<std::string(LogEvent&&)\>
         */
        void setFormatter(MessageType message_type,
                          std::function< std::string(LogEvent&& log_event) >&& formatter);

        /**
         * \overload
         * \brief Sets formatter for specified message type.
         * \tparam Object class which implements the function pointer passed as a second parameter
         * \param message_type MessageType
         * \param object std::shared_ptr\<Object\>
         * \param functor void (Object::*functor)(LogEvent&&)
         */
        template < class Object >
        void setFormatter(MessageType message_type,
                          std::weak_ptr< Object > object,
                          void (Object::*functor)(LogEvent&& log_event));

        /**
         * \overload
         * \brief Sets formatter for specified message type.
         * \tparam Object class which implements the function pointer passed as a second parameter
         * \param message_type MessageType
         * \param object Object*
         * \param functor void (Object::*functor)(LogEvent&&)
         */
        template < class Object >
        void setFormatter(MessageType message_type,
                          Object* object,
                          void (Object::*functor)(LogEvent&& log_event));

        /**
         * \brief Writes log message of preset format to preset output.
         * \param log_event LogEvent&&
         */
        void write(LogEvent&& log_event);

        ~Log() = default;

        static auto createLogInstance() -> std::unique_ptr< Log >;

    private:
        Log();

        /**
         * \internal
         * \brief Mutex to handle multithreaded output if output is set to
         * std::ostream or std::filesystem::path and thus the std::ofstream is used.
         */
        std::mutex m_mutex;

        /**
         * \internal
         * \brief Stores string representations of message types.
         */
        std::vector< std::string > m_message_types;

        /**
         * \internal
         * \brief Stores output for each message type.
         */
        std::vector< std::variant< std::monostate,
                                   std::ostream*,
                                   std::filesystem::path,
                                   std::function< void(const std::string&) > > >
            m_outputs;

        /**
         * \internal
         * \brief Stores formatter functions for each message type.
         */
        std::vector< std::function< std::string(LogEvent&& log_event) > > m_formatters;
    };

    template < class Object >
    void Log::setGlobalOutput(std::weak_ptr< Object > object, void (Object::*functor)(const std::string&)) {
        for (auto& output: m_outputs) {
            output = [object, functor](const std::string& message) {
                if (auto l_object = object.lock()) {
                    std::invoke(functor, l_object, message);
                }
            };
        }
    }

    template < class Object >
    void Log::setGlobalOutput(Object* object, void (Object::*functor)(const std::string&)) {
        for (auto& output: m_outputs) {
            output = [object, functor](const std::string& message) {
                std::invoke(functor, object, message);
            };
        }
    }

    template < class Object >
    void Log::setOutput(MessageType message_type,
                        std::weak_ptr< Object > object,
                        void (Object::*functor)(const std::string&)) {
        m_outputs.at(static_cast< size_t >(message_type)) = [object, functor](const std::string& message) {
            if (auto l_object = object.lock()) {
                std::invoke(functor, l_object, message);
            }
        };
    }

    template < class Object >
    void Log::setOutput(MessageType message_type,
                        Object* object,
                        void (Object::*functor)(const std::string&)) {
        m_outputs.at(static_cast< size_t >(message_type)) = [object, functor](const std::string& message) {
            std::invoke(functor, object, message);
        };
    }

    template < class Object >
    void Log::setGlobalFormatter(std::weak_ptr< Object > object,
                                 void (Object::*functor)(LogEvent&& log_event)) {
        for (auto& formatter: m_formatters) {
            formatter = [object, functor](LogEvent&& log_event) {
                if (auto l_object = object.lock()) {
                    std::invoke(functor, l_object, std::move(log_event));
                }
            };
        }
    }

    template < class Object >
    void Log::setGlobalFormatter(Object* object, void (Object::*functor)(LogEvent&& log_event)) {
        for (auto& formatter: m_formatters) {
            formatter = [object, functor](LogEvent&& log_event) {
                std::invoke(functor, object, std::move(log_event));
            };
        }
    }

    template < class Object >
    void Log::setFormatter(MessageType message_type,
                           std::weak_ptr< Object > object,
                           void (Object::*functor)(LogEvent&& log_event)) {
        m_formatters.at(static_cast< size_t >(message_type)) = [object, functor](LogEvent&& log_event) {
            if (auto l_object = object.lock()) {
                std::invoke(functor, l_object, std::move(log_event));
            }
        };
    }

    template < class Object >
    void Log::setFormatter(MessageType message_type,
                           Object* object,
                           void (Object::*functor)(LogEvent&& log_event)) {
        m_formatters.at(static_cast< size_t >(message_type)) = [object, functor](LogEvent&& log_event) {
            std::invoke(functor, object, std::move(log_event));
        };
    }

    struct LogEvent {
        std::string message_type_string;
        std::string module_name;
        std::string message;
        std::string function_name;
        std::string file_name;
        std::string line;

        std::chrono::time_point< std::chrono::system_clock > time_point;
        std::thread::id thread_id;

        MessageType message_type;

        LogEvent(std::string _module_name,
                 std::string _message,
                 MessageType _message_type,
                 std::string _function_name,
                 std::string _file_name,
                 int _line) :
            module_name(std::move(_module_name)),
            message(std::move(_message)),
            function_name(std::move(_function_name)),
            file_name(std::move(_file_name)),
            line(std::to_string(_line)),
            time_point(std::chrono::system_clock::now()),
            thread_id(std::this_thread::get_id()),
            message_type(_message_type) { }
    };

    //#if defined(LOG_DISABLE_TRACE)
    //  #define WriteTrace(message)
    //#else
    //        /**
    //         * \def WriteTrace(message)
    //         * \brief Convenience macro to output Trace message.
    //         * \param message const std::string&
    //         */
    //  #define WriteTrace(message) \
//    tristan::log::Log::write( \
//        tristan::log::LogEvent(message, tristan::log::MessageType::Trace, __PRETTY_FUNCTION__, __FILE__, __LINE__))
    //#endif
    //
    //#if defined(LOG_DISABLE_DEBUG)
    //  #define WriteDebug(message)
    //#else
    //        /**
    //         * \def WriteDebug(message)
    //         * \brief Convenience macro to output Debug message.
    //         * \param message const std::string&
    //         */
    //  #define WriteDebug(message) \
//    tristan::log::Log::write( \
//        tristan::log::LogEvent(message, tristan::log::MessageType::Debug, __PRETTY_FUNCTION__, __FILE__, __LINE__))
    //#endif
    //
    //#if defined(LOG_DISABLE_ERROR)
    //  #define WriteError(message)
    //#else
    //        /**
    //         * \def WriteError(message)
    //         * \brief Convenience macro to output Error message.
    //         * \param message const std::string&
    //         */
    //  #define WriteError(message) \
//    tristan::log::Log::write( \
//        tristan::log::LogEvent(message, tristan::log::MessageType::Error, __PRETTY_FUNCTION__, __FILE__, __LINE__))
    //#endif
    //
    //#if defined(LOG_DISABLE_WARNING)
    //  #define WriteWarning(message)
    //#else
    //        /**
    //         * \def WriteWarning(message)
    //         * \brief Convenience macro to output Warning message.
    //         * \param message const std::string&
    //         */
    //  #define WriteWarning(message) \
//    tristan::log::Log::write(   \
//        tristan::log::LogEvent(message, tristan::log::MessageType::Warning, __PRETTY_FUNCTION__, __FILE__, __LINE__))
    //#endif
    //
    //#if defined(LOG_DISABLE_INFO)
    //  #define WriteInfo(message)
    //#else
    //        /**
    //         * \def WriteInfo(message)
    //         * \breif Convenience macro to output Info message.
    //         * \param message const std::string&
    //         */
    //  #define WriteInfo(message)  \
//    tristan::log::Log::write( \
//        tristan::log::LogEvent(message, tristan::log::MessageType::Info, __PRETTY_FUNCTION__, __FILE__, __LINE__))
    //#endif
    //
    //#if defined(LOG_DISABLE_FATAL)
    //  #define WriteFatal(message)
    //#else
    //        /**
    //         * \def WriteFatal(message)
    //         * \breif Convenience macro to output Fatal message.
    //         * \param message const std::string&
    //         */
    //  #define WriteFatal(message) \
//    tristan::log::Log::write( \
//        tristan::log::LogEvent(message, tristan::log::MessageType::Fatal, __PRETTY_FUNCTION__, __FILE__, __LINE__))
    //#endif
}  // namespace tristan::log

#endif  // LOG_HPP
