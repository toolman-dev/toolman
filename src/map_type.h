#ifndef MAP_TYPE_H
#define MAP_TYPE_H

#include <string>
#include <utility>
#include "type.h"
#include "primitive_type.h"

namespace toolman {

class MapType : public Type {
    public:
        MapType(const std::string& name, PrimitiveType* key_type, Type* value_type_) 
            :Type(name), key_type_(key_type), value_type_(value_type_) {}
        
        MapType(std::string&& name, PrimitiveType* key_type, Type* value_type_) 
            :Type(std::move(name)), key_type_(key_type), value_type_(value_type_) {}

        bool is_map() const override { return true; }

        PrimitiveType* get_key_type() const { return key_type_; };
        Type* get_value_type() const { return value_type_; };

    private:
        // In toolman, map key must be primitive type.
        PrimitiveType* key_type_;
        Type* value_type_;
};

} // namespace toolman
#endif // MAP_TYPE_H
