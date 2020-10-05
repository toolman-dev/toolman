#ifndef TOOLMAN_MAP_TYPE_H
#define TOOLMAN_MAP_TYPE_H

#include <string>
#include <utility>
#include <memory>
#include "type.h"
#include "primitive_type.h"

namespace toolman {

    class MapType : public Type {
    public:
        MapType(const std::string &name, std::shared_ptr <PrimitiveType> key_type, std::shared_ptr <Type> value_type_)
                : Type(name), key_type_(key_type), value_type_(value_type_) {}

        MapType(std::string &&name, std::shared_ptr <PrimitiveType> key_type, std::shared_ptr <Type> value_type_)
                : Type(std::move(name)), key_type_(key_type), value_type_(value_type_) {}

        bool is_map() const override { return true; }

        const std::shared_ptr <PrimitiveType> &get_key_type() const { return key_type_; };

        const std::shared_ptr <Type> &get_value_type() const { return value_type_; };

    private:
        // In toolman, map key must be primitive type.
        std::shared_ptr <PrimitiveType> key_type_;
        std::shared_ptr <Type> value_type_;
    };

} // namespace toolman
#endif // TOOLMAN_MAP_TYPE_H
