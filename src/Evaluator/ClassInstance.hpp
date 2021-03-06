//
// Created by mafn on 7/25/18.
//

#ifndef HONK_CLASSINSTANCE_HPP
#define HONK_CLASSINSTANCE_HPP

#include <ostream>
#include <unordered_map>
#include <memory>

namespace Honk
{
    struct Class;
    struct Value;

    struct ClassInstance : std::enable_shared_from_this<ClassInstance>
    {
        using s_ptr = std::shared_ptr<ClassInstance>;

        ClassInstance(const Class& cls);

        std::shared_ptr<Value> get_field(std::string identifier);
        bool set_field(std::string identifier, std::shared_ptr<Value> value);

        friend std::ostream& operator<<(std::ostream& os, const ClassInstance& obj);
        friend bool operator==(const ClassInstance& a, const ClassInstance& b);

    private:
        const Class* _class; // Pointer to make this class moveable

        // This is somewhat dubious
        // A class instance is a heap-allocated object, which contains an unordered_map which allocates objects on the
        //     heap, which contains handles (shared pointers) to reference-counted, heap-allocated, objects.
        std::unordered_map<std::string, std::shared_ptr<Value>> _fields;

        void _declare_instance_fields();
        void _declare_field(std::string identifier);

    };
}

#endif //HONK_CLASSINSTANCE_HPP
