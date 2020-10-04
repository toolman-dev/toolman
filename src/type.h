#ifndef TYPE_H
#define TYPE_H

#include <string>
#include <iostream>
namespace toolman {

class Type {
    public:
        virtual void set_name(const std::string& name) { name_ = name; }

        virtual const std::string& get_name() const { return name_; }

        virtual bool is_primitive() const { return false; }
        virtual bool is_enum() const { return false; }
        virtual bool is_struct() const { return false; }
        virtual bool is_list() const { return false; }
        virtual bool is_map() const { return false; }
        
    protected:
        Type(const std::string& name) : name_(name) { }
        Type(std::string&& name) : name_(std::move(name)) { }
        std::string name_;
};

} // namespace v8
#endif // TYPE_H
