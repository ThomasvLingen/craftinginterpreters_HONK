//
// Created by mafn on 1/2/18.
//

#ifndef HONK_UTIL_HPP
#define HONK_UTIL_HPP

#include <string>
#include <optional>
#include <unordered_map>

namespace Honk
{
    namespace Util
    {
        std::string get_file_contents(const std::string& path);

        template<typename Map_K, typename Map_V>
        std::optional<Map_V> map_get_optional(const std::unordered_map<Map_K, Map_V>& map, Map_K key)
        {
            if (map.find(key) == map.cend()) {
                return std::nullopt;
            }

            return map.at(key);
        };
    }
}

#endif //HONK_UTIL_HPP
