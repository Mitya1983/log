#ifndef MT_LOG_HPP
#define MT_LOG_HPP

#include <mutex>
#include <filesystem>
#include <functional>
#include <fstream>
#include <iostream>
#include <string>
#include <variant>
#include <array>
#include <atomic>
#include <source_location>

namespace mt::log {

    auto processID() -> uint64_t;

    /**
     * \enum message_type
     * \brief List of supported message types
     */
    enum class message_type : uint8_t {
        Trace,
        Debug,
        Error,
        Warning,
        Info,
        Fatal
    };

    static constexpr uint8_t message_type_count = 6;

    struct log_event {
        log_event(std::string p_module,
                  std::string p_message,
                  message_type p_message_type,
                  std::string p_function_name,
                  std::string p_file_name,
                  uint32_t p_line);
        log_event(std::string p_module, std::string p_message, enum message_type p_message_type, std::source_location p_source_location);
        log_event(std::string_view p_module,
                  std::string_view p_message,
                  message_type p_message_type,
                  std::string p_function_name,
                  std::string p_file_name,
                  uint32_t p_line);
        log_event(std::string_view p_module, std::string_view p_message, enum message_type p_message_type, std::source_location p_source_location);

        log_event(const log_event& other) = delete;
        log_event(log_event&& other) = default;

        log_event& operator=(const log_event& other) = delete;
        log_event& operator=(log_event&& other) = default;

        ~log_event() = default;

        [[nodiscard]] auto to_string(const std::function< std::string(const log_event&) >& formatter = {}) const -> std::string;

        std::chrono::time_point< std::chrono::system_clock > time_point;
        std::string module_name;
        std::string message_type_string;
        std::string message;
        std::string function_name;
        std::string file_name;
        std::string line;

        enum message_type message_type;
    };

    /**
     * \class log
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
     * defined callbacks. That is, it is a user obligation to handle multi-threaded
     * calls of provided callback function.
     */
    template < class mutex = std::mutex > class log {
    public:
        log() = default;

        log(const log&) = delete;
        log(log&&) = delete;
        log& operator=(const log&) = delete;
        log& operator=(log&&) = delete;

        void set_message_type_output(message_type message_type, const std::string& value) { m_message_types.at(static_cast< size_t >(message_type)) = value; }

        void set_global_output(std::ostream* output_stream) {
            for (auto& output: m_outputs) {
                if (output_stream == nullptr) {
                    output = std::monostate();
                } else {
                    output = output_stream;
                }
            }
        }

        void set_global_output(const std::filesystem::path& file) {
            for (auto& output: m_outputs) {
                output = file;
            }
        }

        void set_global_output(std::function< void(const std::string&) >&& output_func) {
            for (auto& l_output: m_outputs) {
                if (output_func == nullptr) {
                    l_output = std::monostate();
                } else {
                    l_output = output_func;
                }
            }
        }

        template < class Object > void set_global_output(std::weak_ptr< Object > object, void (Object::*functor)(const std::string&)) {
            for (auto& output: m_outputs) {
                output = [object, functor](const std::string& message) {
                    if (auto l_object = object.lock()) {
                        std::invoke(functor, l_object, message);
                    }
                };
            }
        }

        template < class Object > void set_global_output(Object* object, void (Object::*functor)(const std::string&)) {
            for (auto& output: m_outputs) {
                output = [object, functor](const std::string& message) {
                    std::invoke(functor, object, message);
                };
            }
        }

        void set_output(message_type message_type, std::ostream* output_stream) {
            if (output_stream == nullptr) {
                m_outputs.at(static_cast< size_t >(message_type)) = std::monostate();
            } else {
                m_outputs.at(static_cast< size_t >(message_type)) = output_stream;
            }
        }

        void set_output(message_type message_type, const std::filesystem::path& file) { m_outputs.at(static_cast< size_t >(message_type)) = file; }

        void set_output(message_type message_type, std::function< void(const std::string&) >&& output_func) {
            if (output_func == nullptr) {
                m_outputs.at(static_cast< size_t >(message_type)) = std::monostate();
            } else {
                m_outputs.at(static_cast< size_t >(message_type)) = output_func;
            }
        }

        template < class Object > void set_output(message_type message_type, std::weak_ptr< Object > object, void (Object::*functor)(const std::string&)) {
            m_outputs.at(static_cast< size_t >(message_type)) = [object, functor](const std::string& message) {
                if (auto l_object = object.lock()) {
                    std::invoke(functor, l_object, message);
                }
            };
        }

        template < class Object > void set_output(message_type message_type, Object* object, void (Object::*functor)(const std::string&)) {
            m_outputs.at(static_cast< size_t >(message_type)) = [object, functor](const std::string& message) {
                std::invoke(functor, object, message);
            };
        }

        void set_global_formatter(std::function< std::string(const log_event& log_event) >&& formatter) {
            for (auto& l_formatter: m_formatters) {
                l_formatter = formatter;
            }
        }

        void set_formatter(message_type message_type, std::function< std::string(const log_event& log_event) >&& formatter) {
            m_formatters.at(static_cast< size_t >(message_type)) = formatter;
        }

        void write(log_event&& log_event) {
#if defined(LOG_DISABLE_TRACE)
            if (log_event.message_type == message_type::Trace) {
                return;
            }
#endif
#if defined(LOG_DISABLE_DEBUG)
            if (log_event.message_type == message_type::Debug) {
                return;
            }
#endif
#if defined(LOG_DISABLE_ERROR)
            if (log_event.message_type == message_type::Error) {
                return;
            }
#endif
#if defined(LOG_DISABLE_WARNING)
            if (log_event.message_type == message_type::Warning) {
                return;
            }
#endif
#if defined(LOG_DISABLE_INFO)
            if (log_event.message_type == message_type::Info) {
                return;
            }
#endif
#if defined(LOG_DISABLE_FATAL)
            if (log_event.message_type == message_type::Fatal) {
                return;
            }
#endif
            auto message_index = m_message_index.fetch_add(1, std::memory_order_relaxed);
            const auto message_type_index = static_cast< uint64_t >(log_event.message_type);
            log_event.message_type_string = m_message_types.at(message_type_index);
            std::string msg;
            if (m_include_proc_id) {
                msg += std::to_string(processID()) + "-";
            }
            msg += std::to_string(message_index) + ": ";
            if (const auto& formatter = m_formatters.at(message_type_index); formatter) {
                msg += log_event.to_string(formatter);
            } else {
                msg += log_event.to_string();
            }
            std::visit(
                [this, &msg]< typename DestinationType >(DestinationType&& arg) -> void {
                    using T = std::decay_t< DestinationType >;
                    if constexpr (std::is_same_v< T, std::ostream* >) {
                        std::scoped_lock lock(m_mutex);
                        arg->write(msg.data(), std::ssize(msg));
                        arg->flush();
                    } else if constexpr (std::is_same_v< T, std::filesystem::path >) {
                        std::scoped_lock lock(m_mutex);
                        std::ofstream file(arg, std::ios::app);
                        if (not file.is_open()) {
                            throw std::fstream::failure("Could not open Log file for writing - ", std::error_code(errno, std::system_category()));
                        }
                        file.write(msg.data(), std::ssize(msg));
                        file.flush();
                        file.close();
                    } else if constexpr (std::is_same_v< T, std::function< void(const std::string&) > >) {
                        arg(msg);
                    }
                },
                m_outputs.at(message_type_index));
        }

        ~log() = default;

    private:
        mutex m_mutex;

        std::array< std::string, message_type_count > m_message_types{"TRACE", "DEBUG", "ERROR", "WARNING", "INFO", "FATAL"};
        std::array< std::variant< std::monostate, std::ostream*, std::filesystem::path, std::function< void(const std::string&) > >, message_type_count >
            m_outputs{&std::cout, &std::cout, &std::cout, &std::cout, &std::cout, &std::cout};
        std::array< std::function< std::string(const log_event& log_event) >, message_type_count > m_formatters{};

        std::atomic< int32_t > m_message_index{0};
        bool m_include_proc_id{false};
    };

}  // namespace mt::log

#endif  // MT_LOG_HPP
