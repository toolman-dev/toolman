#ifndef TOOLMAN_ENUM_FIELD_H
#define TOOLMAN_ENUM_FIELD_H

#include <memory>
#include <string>
#include "type.h"

namespace toolman {
    class EnumField {
    public:
        EnumField(const std::string &name, int value)
                : Type(name), value_(value) {}

        EnumField(std::string &&name, int value)
                : Type(std::move(name)), value_(value) {}

        const std::string &get_name() const { return name_; }

        int get_value() const { return value_; }

    private:
        std::string name_;
        int value_;
    };
} // namespace toolman

#endif //TOOLMAN_ENUM_FIELD_H
