#ifndef STRUCT_FIELD_H
#define STRUCT_FIELD_H

#include <memory>

#include "type.h"

namespace toolman {

class StructField {
    public:
        StructField(std::shared_ptr<Type> type, const std::string& name, bool optional) 
            : type_(type), name_(name), optional_(optional) {}

        StructField(std::shared_ptr<Type> type, const std::string&& name, bool optional) 
            : type_(type), name_(std::move(name)), optional_(optional) {}
        
        const std::string& get_name() const { return name_; }

        bool is_optional() const { return optional_; }

    private:
        std::shared_ptr<Type> type_;
        std::string name_;
        bool optional_;
};

} // namespace toolman

#endif // STRUCT_FIELD_H
