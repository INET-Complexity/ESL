

#ifndef ESL_DATA_FILE_HPP
#define ESL_DATA_FILE_HPP


#include <fstream>

#include <esl/data/filesystem.hpp>


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
        explicit file(const std::filesystem::path &name, const std::filesystem::path &prefix);
    };
}//esl::data


#endif //ESL_DATA_FILE_HPP
