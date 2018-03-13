//
// Created by mafn on 3/13/18.
//

#ifndef HONK_MISC_HPP
#define HONK_MISC_HPP

#include <string>

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
    }
}

#endif //HONK_MISC_HPP
