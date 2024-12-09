#ifndef MT_LOG_HPP
#define MT_LOG_HPP

#include <mutex>
#include <filesystem>
#include <functional>
#include <optional>
#include <ostream>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <variant>
#include <vector>
#include <source_location>
namespace mt::log {

    auto processID() -> uint64_t;

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

    struct LogEvent {
        /**
         *
         * \param p_message std::string
         * \param p_message_type tristan::log::MessageType
         * \param p_function_name std::string
         * \param p_file_name std::string
         * \param p_line int
         */
        LogEvent(std::string p_message, MessageType p_message_type, std::string p_function_name, std::string p_file_name, uint32_t p_line);
        /**
         * \param p_message std::string
         * \param p_message_type MessageType
         * \param p_source_location std::source_location
         */
        LogEvent(std::string p_message, MessageType p_message_type, std::source_location p_source_location);

        LogEvent(const LogEvent& other) = delete;
        LogEvent(LogEvent&& other) = default;

        LogEvent& operator=(const LogEvent& other) = delete;
        LogEvent& operator=(LogEvent&& other) = default;

        ~LogEvent();

        [[nodiscard]] auto toString(const std::function< std::string(const LogEvent&) >& formatter = {}) const -> std::string;

        std::chrono::time_point< std::chrono::system_clock > time_point;
        std::string message_type_string;
        std::string module_name;
        std::string message;
        std::string function_name;
        std::string file_name;
        std::string line;
        std::thread::id thread_id;

        MessageType message_type;
    };

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
    template < class IPCMutex, class ThreadMutex = std::mutex > class Log {
    public:
        Log() {
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
        }

        Log(const Log&) = delete;
        Log(Log&&) = delete;
        Log& operator=(const Log&) = delete;
        Log& operator=(Log&&) = delete;

        void setIpcMutex(IPCMutex&& p_ipc_mutex) { m_ipc_mutex = std::move(p_ipc_mutex); }

        /**
         * \brief Sets module name
         * \param name std::string
         */
        void setModuleName(std::string name) { m_module_name = std::move(name); }

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
        void setMessageTypeOutput(MessageType message_type, const std::string& value) { m_message_types.at(static_cast< size_t >(message_type)) = value; }

        /**
         * \brief Sets output for all message types.
         * \param output_stream std::ostream*
         */
        void setGlobalOutput(std::ostream* output_stream) {
            for (auto& output: m_outputs) {
                if (output_stream == nullptr) {
                    output = std::monostate();
                } else {
                    output = output_stream;
                }
            }
        }

        /**
         * \overload
         * \brief Sets output for all message types.
         * \param file const std::filesystem::path&
         */
        void setGlobalOutput(const std::filesystem::path& file) {
            for (auto& output: m_outputs) {
                output = file;
            }
        }

        /**
         * \overload
         * \brief Sets output for all message types.
         * \param output_func std::function\<void(const std::string&)\>&&
         */
        void setGlobalOutput(std::function< void(const std::string&) >&& output_func) {
            for (auto& l_output: m_outputs) {
                if (output_func == nullptr) {
                    l_output = std::monostate();
                } else {
                    l_output = output_func;
                }
            }
        }

        /**
         * \overload
         * \brief Sets output for all message types.
         * \tparam Object class which implements the function pointer passed as a
         * second parameter \param object std::shared_ptr\<Object\> \param functor
         * void (Object::*functor)(const std::string&)
         */
        template < class Object > void setGlobalOutput(std::weak_ptr< Object > object, void (Object::*functor)(const std::string&)) {
            for (auto& output: m_outputs) {
                output = [object, functor](const std::string& message) {
                    if (auto l_object = object.lock()) {
                        std::invoke(functor, l_object, message);
                    }
                };
            }
        }

        /**
         * \overload
         * \brief Sets output for all message types.
         * \tparam Object class which implements the function pointer passed as a
         * second parameter \param object Object* \param functor void
         * (Object::*functor)(const std::string&)
         */
        template < class Object > void setGlobalOutput(Object* object, void (Object::*functor)(const std::string&)) {
            for (auto& output: m_outputs) {
                output = [object, functor](const std::string& message) {
                    std::invoke(functor, object, message);
                };
            }
        }

        /**
         * \brief Sets output for specified message type.
         * \param message_type MessageType
         * \param output_stream std::ostream*
         */
        void setOutput(MessageType message_type, std::ostream* output_stream) {
            if (output_stream == nullptr) {
                m_outputs.at(static_cast< size_t >(message_type)) = std::monostate();
            } else {
                m_outputs.at(static_cast< size_t >(message_type)) = output_stream;
            }
        }

        /**
         * \overload
         * \brief Sets output for specified message type.
         * \param message_type MessageType
         * \param file const std::filesystem::path&
         */

        void setOutput(MessageType message_type, const std::filesystem::path& file) { m_outputs.at(static_cast< size_t >(message_type)) = file; }

        /**
         * \overload
         * \brief Sets output for specified message type.
         * \param message_type MessageType
         * \param output_func std::function\<void(const std::string&)\>&&
         */
        void setOutput(MessageType message_type, std::function< void(const std::string&) >&& output_func) {
            if (output_func == nullptr) {
                m_outputs.at(static_cast< size_t >(message_type)) = std::monostate();
            } else {
                m_outputs.at(static_cast< size_t >(message_type)) = output_func;
            }
        }

        /**
         * \overload
         * \brief Sets output for specified message type.
         * \tparam Object class which implements the function pointer passed as a second parameter
         * \param message_type MessageType
         * \param object std::shared_ptr\<Object\>
         * \param functor void (Object::*functor)(const std::string&)
         */
        template < class Object > void setOutput(MessageType message_type, std::weak_ptr< Object > object, void (Object::*functor)(const std::string&)) {
            m_outputs.at(static_cast< size_t >(message_type)) = [object, functor](const std::string& message) {
                if (auto l_object = object.lock()) {
                    std::invoke(functor, l_object, message);
                }
            };
        }

        /**
         * \overload
         * \brief Sets output for specified message type.
         * \tparam Object class which implements the function pointer passed as a second parameter
         * \param message_type MessageType \param object Object*
         * \param functor void (Object::*functor)(const std::string&)
         */
        template < class Object > void setOutput(MessageType message_type, Object* object, void (Object::*functor)(const std::string&)) {
            m_outputs.at(static_cast< size_t >(message_type)) = [object, functor](const std::string& message) {
                std::invoke(functor, object, message);
            };
        }

        /**
         * \brief Sets formatter for all message types.
         * \param formatter std::function\<std::string(LogEvent&&)\>
         */
        void setGlobalFormatter(std::function< std::string(const LogEvent& log_event) >&& formatter) {
            for (auto& l_formatter: m_formatters) {
                l_formatter = formatter;
            }
        }

        /**
         * \brief Sets formatter for specified message type.
         * \param message_type MessageType
         * \param formatter std::function\<std::string(LogEvent&&)\>
         */
        void setFormatter(MessageType message_type, std::function< std::string(const LogEvent& log_event) >&& formatter) {
            m_formatters.at(static_cast< size_t >(message_type)) = formatter;
        }

        /**
         * \brief Writes log message of preset format to preset output.
         * \param log_event LogEvent&&
         */
        void write(LogEvent&& log_event) {
#if defined(LOG_DISABLE_TRACE)
            if (log_event.message_type == MessageType::Trace) {
                return;
            }
#endif
#if defined(LOG_DISABLE_DEBUG)
            if (log_event.message_type == MessageType::Debug) {
                return;
            }
#endif
#if defined(LOG_DISABLE_ERROR)
            if (log_event.message_type == MessageType::Error) {
                return;
            }
#endif
#if defined(LOG_DISABLE_WARNING)
            if (log_event.message_type == MessageType::Warning) {
                return;
            }
#endif
#if defined(LOG_DISABLE_INFO)
            if (log_event.message_type == MessageType::Info) {
                return;
            }
#endif
#if defined(LOG_DISABLE_FATAL)
            if (log_event.message_type == MessageType::Fatal) {
                return;
            }
#endif
            static int32_t message_index{0};
            const auto message_type_index = static_cast< uint64_t >(log_event.message_type);
            log_event.message_type_string = m_message_types.at(message_type_index);
            log_event.module_name = m_module_name;
            auto formatter = m_formatters.at(message_type_index);
            std::string msg = std::to_string(processID()) + "-" + std::to_string(message_index) + ": ";
            msg += log_event.toString(m_formatters.at(message_type_index));
            std::visit(
                [this, &msg]< typename DestinationType >(DestinationType&& arg) -> void {
                    using T = std::decay_t< DestinationType >;
                    if constexpr (std::is_same_v< T, std::ostream* >) {
                        std::scoped_lock lock(m_mutex);
                        arg->write(msg.data(), std::ssize(msg));
                    } else if constexpr (std::is_same_v< T, std::filesystem::path >) {
                        std::scoped_lock lock(m_mutex);
                        if (m_ipc_mutex != std::nullopt) {
                            m_ipc_mutex.lock();
                        }
                        std::ofstream file(arg, std::ios::app);
                        if (not file.is_open()) {
                            if (m_ipc_mutex != std::nullopt) {
                                m_ipc_mutex.unlock();
                            }
                            throw std::fstream::failure("Could not open Log file for writing - ", std::error_code(errno, std::system_category()));
                        }
                        file.write(msg.data(), std::ssize(msg));
                        file.close();
                        if (m_ipc_mutex != std::nullopt) {
                            m_ipc_mutex.unlock();
                        }
                    } else if constexpr (std::is_same_v< T, std::function< void(const std::string&) > >) {
                        arg(msg);
                    }
                },
                m_outputs.at(message_type_index));
            ++message_index;
        }

        ~Log() = default;

    private:
        ThreadMutex m_mutex;
        std::optional< IPCMutex > m_ipc_mutex{std::nullopt};
        std::string m_module_name;

        /**
         * \internal
         * \brief Stores string representations of message types.
         */
        std::vector< std::string > m_message_types;

        /**
         * \internal
         * \brief Stores output for each message type.
         */
        std::vector< std::variant< std::monostate, std::ostream*, std::filesystem::path, std::function< void(const std::string&) > > > m_outputs;

        /**
         * \internal
         * \brief Stores formatter functions for each message type.
         */
        std::vector< std::function< std::string(const LogEvent& log_event) > > m_formatters;
    };

}  // namespace mt::log

#endif  // MT_LOG_HPP
