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

namespace tristan::date_time {
    class DateTime;
} //End of tristan::time namespace

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
         * \brief Sets module name
         * \param name std::string
         */
        void setModuleName(std::string name);

        /**
         * \brief Sets name for IPC lock, aka sem.[name]
         * \param name std::string
         */
        void setIpcLockName(std::string name);

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
                                std::string (Object::*functor)(LogEvent&& log_event));

        /**
         * \overload
         * \brief Sets formatter for all message types.
         * \tparam Object class which implements the function pointer passed as a second parameter
         * \param object Object*
         * \param functor void (Object::*functor)(LogEvent&&)
         */
        template < class Object >
        void setGlobalFormatter(Object* object, std::string (Object::*functor)(LogEvent&& log_event));

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
                          std::string (Object::*functor)(LogEvent&& log_event));

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
                          std::string (Object::*functor)(LogEvent&& log_event));

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

        std::string m_module_name;
        std::string m_ipc_lock_name;

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
                                 std::string (Object::*functor)(LogEvent&& log_event)) {
        for (auto& formatter: m_formatters) {
            formatter = [object, functor](LogEvent&& log_event) -> std::string {
                if (auto l_object = object.lock()) {
                    return std::invoke(functor, l_object, std::move(log_event));
                }
            };
        }
    }

    template < class Object >
    void Log::setGlobalFormatter(Object* object, std::string (Object::*functor)(LogEvent&& log_event)) {
        for (auto& formatter: m_formatters) {
            formatter = [object, functor](LogEvent&& log_event) -> std::string {
                return std::invoke(functor, object, std::move(log_event));
            };
        }
    }

    template < class Object >
    void Log::setFormatter(MessageType message_type,
                           std::weak_ptr< Object > object,
                           std::string (Object::*functor)(LogEvent&& log_event)) {
        m_formatters.at(static_cast< size_t >(message_type)) = [object, functor](LogEvent&& log_event) -> std::string {
            if (auto l_object = object.lock()) {
                return std::invoke(functor, l_object, std::move(log_event));
            }
        };
    }

    template < class Object >
    void Log::setFormatter(MessageType message_type,
                           Object* object,
                           std::string (Object::*functor)(LogEvent&& log_event)) {
        m_formatters.at(static_cast< size_t >(message_type)) = [object, functor](LogEvent&& log_event) -> std::string {
            return std::invoke(functor, object, std::move(log_event));
        };
    }

    struct LogEvent {
        std::string message_type_string;
        std::string module_name;
        std::string message;
        std::string function_name;
        std::string file_name;
        std::string line;

        std::unique_ptr<tristan::date_time::DateTime> time_point;
        std::thread::id thread_id;

        MessageType message_type;

        /**
         *
         * \param p_message std::string
         * \param p_message_type tristan::log::MessageType
         * \param p_function_name std::string
         * \param p_file_name std::string
         * \param p_line int
         */
        LogEvent(std::string p_message,
                 MessageType p_message_type,
                 std::string p_function_name,
                 std::string p_file_name,
                 uint32_t p_line);
    };

}  // namespace tristan::log

#endif  // LOG_HPP
