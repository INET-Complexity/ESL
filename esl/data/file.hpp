

#ifndef ESL_DATA_FILE_HPP
#define ESL_DATA_FILE_HPP

#ifndef __APPLE__
#include <filesystem>
#endif

#include <fstream>

#include <esl/data/stream.hpp>



namespace esl::data {

    class file
    : public stream
    {
    private:
        std::ofstream file_;

    public:
        ///
        /// TODO: move to parametrization input directory and output directory
        ///
        /// \param x
#ifndef __APPLE__   // macos-latest with XCode 12 does not yet support this
        explicit file(const std::filesystem::path &name, const std::filesystem::path &prefix);
#else
        explicit file(const std::string  &name, const std::string &prefix);
#endif
    };
}//esl::data


#endif //ESL_DATA_FILE_HPP
