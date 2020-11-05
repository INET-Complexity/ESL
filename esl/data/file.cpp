#include <esl/data/file.hpp>
#include <esl/data/log.hpp>

#include <sstream>


namespace esl::data {

#ifndef __APPLE__
    file::file(const std::filesystem::path &name, const std::filesystem::path &prefix)
    : stream(file_)
    , file_((std::filesystem::create_directories(prefix), prefix / name ))
    {
        if(!file_.good()){
            std::stringstream message_;
            message_ << "Can't create output file " << (prefix / name);
            LOG(errorlog) << message_.str() << std::endl;
            throw std::runtime_error(message_.str());
        }
    }
#else
    file::file(const std::string &name, const std::string &prefix)
    : stream(file_)
    //, file_((std::filesystem::create_directories(prefix), prefix / name ))
    {
        file_.open(prefix + "/" + name);

        if(!file_.good()){
            std::stringstream message_;
            message_ << "Can't create output file " << (prefix + "/" + name);
            LOG(errorlog) << message_.str() << std::endl;
            throw std::runtime_error(message_.str());
        }
    }
#endif
}