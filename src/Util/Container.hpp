//
// Created by mafn on 3/13/18.
//

#ifndef HONK_CONTAINER_HPP
#define HONK_CONTAINER_HPP

#include <optional>
#include <unordered_map>
#include <algorithm>

namespace Honk
{
    namespace Util
    {
        template<typename Map_K, typename Map_V>
        std::optional<Map_V> map_get_optional(const std::unordered_map<Map_K, Map_V>& map, Map_K key)
        {
            if (map.find(key) == map.cend()) {
                return std::nullopt;
            }

            return map.at(key);
        };

        template <typename Map_K, typename Map_V>
        __maybe_nullptr __non_owning Map_V* map_get_ptr(std::unordered_map<Map_K, Map_V>& map, Map_K key)
        {
            if (map.find(key) == map.cend()) {
                return nullptr;
            }

            return &map.at(key);
        };

        template <typename Container, typename Value>
        bool contains(const Container& container, const Value& val)
        {
            return std::find(container.begin(), container.end(), val) != container.end();
        };
    }
}

#endif //HONK_CONTAINER_HPP