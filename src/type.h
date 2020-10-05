#ifndef TOOLMAN_TYPE_H
#define TOOLMAN_TYPE_H

#include <string>
#include <utility>

namespace toolman {

    class Type {
    public:

        virtual const std::string &get_name() const { return name_; }

        virtual bool is_primitive() const { return false; }

        virtual bool is_enum() const { return false; }

        virtual bool is_struct() const { return false; }

        virtual bool is_list() const { return false; }

        virtual bool is_map() const { return false; }

    protected:
        explicit Type(const std::string &name) : name_(name) {}

        explicit Type(std::string &&name) : name_(std::move(name)) {}

        std::string name_;
    };

} // namespace toolman
#endif // TOOLMAN_TYPE_H
