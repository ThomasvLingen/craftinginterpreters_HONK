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
    }
}
