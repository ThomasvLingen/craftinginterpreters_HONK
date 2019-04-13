//
// Created by mafn on 3/13/18.
//

#ifndef HONK_MISC_HPP
#define HONK_MISC_HPP

#include <string>
#include <cstdint>

// Everything in this file should probably reside in a different file.

namespace Honk
{
    namespace Util
    {
        std::string get_file_contents(const std::string& path);

        inline const char* bool_str(bool val)
        {
            if (val) {
                return "true";
            } else {
                return "false";
            }
        }

        int64_t stoint64_t(const std::string& src);
    }
}

#endif //HONK_MISC_HPP
