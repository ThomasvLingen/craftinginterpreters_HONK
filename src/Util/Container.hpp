//
// Created by mafn on 3/13/18.
//

#ifndef HONK_CONTAINER_HPP
#define HONK_CONTAINER_HPP

#include <optional>
#include <unordered_map>
#include <map>
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

        template <typename Map_K, typename Map_V>
        __maybe_nullptr __non_owning Map_V map_get_s_ptr(std::unordered_map<Map_K, Map_V>& map, Map_K key)
        {
            if (map.find(key) == map.cend()) {
                return nullptr;
            }

            return map.at(key);
        };

        template <typename Container, typename Value>
        bool contains(const Container& container, const Value& val)
        {
            return std::find(container.begin(), container.end(), val) != container.end();
        };

        template <typename Map_K, typename Map_V>
        bool contains(const std::map<Map_K, Map_V>& map, const Map_K& key)
        {
            return map.find(key) != map.cend();
        };

        template <typename Map_K, typename Map_V>
        bool contains(const std::unordered_map<Map_K, Map_V>& map, const Map_K& key)
        {
            return map.find(key) != map.cend();
        };

        template <typename TargetContainer, typename Container, typename Callable>
        TargetContainer map(Container& original, const Callable& map_fn)
        {
            using mapped_type = typename Container::value_type;
            TargetContainer mapped_values;

            for (mapped_type& value : original) {
                mapped_values.push_back(map_fn(value));
            }

            return mapped_values;
        }

        template <typename TargetMap, typename Container, typename Callable>
        TargetMap map_to_stlmap(const Container& original, const Callable& map_fn)
            // Signature of map_fn = std::pair<std::string, TargetMap::mapped_type> map_fn(const Container::value_type& value)
        {
            using mapped_type = typename Container::value_type;
            TargetMap mapped_values;

            for (const mapped_type& value : original) {
                mapped_values.insert(map_fn(value));
            }

            return mapped_values;
        }
    }
}

#endif //HONK_CONTAINER_HPP
