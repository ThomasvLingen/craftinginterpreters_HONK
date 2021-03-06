//
// Created by mafn on 3/13/18.
//

#include "Misc.hpp"

#include <fstream>
#include <sstream>

namespace Honk
{
    namespace Util
    {
        std::string get_file_contents(const std::string& path)
        {
            std::ifstream file(path);
            std::stringstream string_stream;

            // This looks a bit stupid, but it just werks
            string_stream << file.rdbuf();

            return string_stream.str();
        }

        int64_t stoint64_t(const std::string& src)
        {
            if constexpr (std::is_same_v<int64_t, long>) {
                return std::stol(src);
            }

            if constexpr (std::is_same_v<int64_t, long long>) {
                return std::stoll(src);
            }
        }
    }
}
