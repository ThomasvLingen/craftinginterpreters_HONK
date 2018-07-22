//
// Created by mafn on 7/23/18.
//

#ifndef HONK_CLASS_HPP
#define HONK_CLASS_HPP

#include <string>

namespace Honk
{
    struct Class
    {
        Class(std::string name);

        std::string name;

        friend std::ostream& operator<<(std::ostream& os, const Class& obj);
        friend bool operator==(const Class& a, const Class& b);
    };
}


#endif //HONK_CLASS_HPP
