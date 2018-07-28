//
// Created by mafn on 7/25/18.
//

#ifndef HONK_CLASSINSTANCE_HPP
#define HONK_CLASSINSTANCE_HPP

#include <ostream>

namespace Honk
{
    struct Class;

    struct ClassInstance
    {
        ClassInstance(const Class& cls);

        friend std::ostream& operator<<(std::ostream& os, const ClassInstance& obj);
        friend bool operator==(const ClassInstance& a, const ClassInstance& b);

    private:
        const Class* _class; // Pointer to make this class moveable
    };
}

#endif //HONK_CLASSINSTANCE_HPP
