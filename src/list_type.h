#ifndef TOOLMAN_LIST_TYPE_H
#define TOOLMAN_LIST_TYPE_H

#include <string>
#include <memory>
#include "type.h"

namespace toolman {

    class ListType : public Type {
    public:
        ListType(const std::string &name, std::shared_ptr <Type> elem_type)
                : Type(name), elem_type_(elem_type) {}

        ListType(std::string &&name, std::shared_ptr <Type> elem_type)
                : Type(std::move(name)), elem_type_(elem_type) {}

        const std::shared_ptr <Type>& get_elem_type() const { return elem_type_; }

        bool is_list() const override { return true; }

    private:
        std::shared_ptr <Type> elem_type_;
    };

} // namespace toolman
#endif // TOOLMAN_LIST_TYPE_H