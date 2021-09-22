

#include "log.hpp"
#include "date_time.hpp"
#include <fstream>
#include <iostream>

#if defined (_WIN32) || defined(_WIN64)
#include <cerrno.h>
#endif

using namespace tristan::log;

namespace
{

} //End of unnammed namespace

Log::Log()
{
    m_message_types.emplace(MessageType::Debug, "DEBUG");
    m_message_types.emplace(MessageType::Error, "ERROR");
    m_message_types.emplace(MessageType::Info, "INFO");
    m_message_types.emplace(MessageType::Fatal, "FATAL");
    m_message_types.emplace(MessageType::Trace, "TRACE");
    m_message_types.emplace(MessageType::Warning, "WARNING");

    m_outputs.emplace(MessageType::Debug, OutputStream::Cout);
    m_outputs.emplace(MessageType::Error, OutputStream::Cerr);
    m_outputs.emplace(MessageType::Info, OutputStream::Cout);
    m_outputs.emplace(MessageType::Fatal, OutputStream::Cerr);
    m_outputs.emplace(MessageType::Trace, OutputStream::Cout);
    m_outputs.emplace(MessageType::Warning, OutputStream::Cout);
}

void Log::createLogDirectory(const std::filesystem::path& directory){
    
    if (!std::filesystem::exists(directory)){
        try{
            std::filesystem::create_directories(directory);
        }
        catch (const std::filesystem::filesystem_error &e){
            std::cerr << Log::instance().m_message_types.at(MessageType::Warning) << ": " << __PRETTY_FUNCTION__ << " - " << e.what() << std::endl;
        }
    }
}

void Log::setGlobalOutput(OutputStream output_stream){
    auto &outputs = Log::instance().m_outputs;
    for (auto &output :  outputs){
        output.second = output_stream;
    }
}

void Log::setGlobalOutput(const std::filesystem::path& file){
    auto &outputs = Log::instance().m_outputs;
    for (auto &output :  outputs){
        output.second = file;
    }
}

void Log::setOutput(MessageType message_type, OutputStream output_stream){
    Log::instance().m_outputs.at(message_type) = output_stream;
}

void Log::setOutput(MessageType message_type, const std::filesystem::path &file){
    Log::instance().m_outputs.at(message_type) = file;
}

void Log::write(std::string_view message, MessageType message_type, std::string_view function_name, std::string_view file, int line){
    
    try{
        Log::instance()._write(message, message_type, function_name, file, std::to_string(line));
    }
    catch(const std::fstream::failure &e){
        std::cerr << Log::instance().m_message_types.at(MessageType::Warning) << ": " << __PRETTY_FUNCTION__ << " - " << e.what() << std::endl;
    }
    
}

void Log::_write(std::string_view message, MessageType message_type, std::string_view function_name, std::string_view file, std::string_view line){
    
    tristan::date_time::Date_Time current;
    std::string msg = m_message_types.at(message_type) + "\t" + current.toString() + "\t";
    msg += function_name;
    msg += " - ";
    msg += message;
    msg += " at file  ";
    msg += file;
    msg += " line ";
    msg += line;
    std::visit(
            [this, &msg](auto && arg) -> void{
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, OutputStream>){
                    std::ostream* output;
                    if (arg == OutputStream::Cout){
                        output = &std::cout;
                    }
                    else{
                        output = &std::cerr;
                    }
                    std::unique_lock<std::mutex> lock(m_mutex);
                    *output << msg << std::endl;
                }
                else{
                    std::unique_lock<std::mutex> lock(m_mutex);
                    std::ofstream file(arg, std::ios::app);
                    if (!file.is_open()){
                        std::cerr << Log::instance().m_message_types.at(MessageType::Warning)<< ": " << __PRETTY_FUNCTION__ << " - "
                                  << "Could not create log file - " + std::error_code(errno, std::system_category()).message() << std::endl;
                        return;
                    }
                    file << msg << std::endl;
                    file.close();
                }
                }
            , m_outputs.at(message_type)
            );
}

auto Log::instance() -> Log&{
    
    static Log log;
    return log;
}



