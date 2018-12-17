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

    struct ClassInstance
    {
        ClassInstance(const Class& cls);

        std::shared_ptr<Value> get_field(std::string identifier);
        void set_field(std::string identifier, std::shared_ptr<Value> value);

        friend std::ostream& operator<<(std::ostream& os, const ClassInstance& obj);
        friend bool operator==(const ClassInstance& a, const ClassInstance& b);

    private:
        const Class* _class; // Pointer to make this class moveable

        // This is somewhat dubious
        // A class instance is a heap-allocated object, which contains an unordered_map which allocates objects on the
        //     heap, which contains handles (shared pointers) to reference-counted, heap-allocated, objects.
        std::unordered_map<std::string, std::shared_ptr<Value>> _fields;
    };
}

#endif //HONK_CLASSINSTANCE_HPP
