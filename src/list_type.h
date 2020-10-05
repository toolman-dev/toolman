#ifndef LIST_TYPE_H
#define LIST_TYPE_H

#include <string>
#include "type.h"

namespace toolman
{

    class ListType : public Type
    {
    public:
        ListType(const std::string &name, Type *elem_type) : Type(name), elem_type_(elem_type) {}
        ListType(std::string &&name, Type *elem_type)
            : Type(std::move(name)), elem_type_(elem_type) {}

        Type *get_elem_type() const { return elem_type_; }

        bool is_list() const override { return true; }

    private:
        Type *elem_type_;
    };

} // namespace toolman
#endif // LIST_TYPE_H