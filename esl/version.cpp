
#include <esl/version.hpp>

#include <sstream>
using std::stringstream;

namespace esl {
    string version()
    {
        stringstream stream_;
        stream_ << ESL_VERSION_MAJOR << '.';
        stream_ << ESL_VERSION_MINOR << '.';
        stream_ << ESL_VERSION_REVISION;
        return stream_.str();
    }
}//namespace esl
