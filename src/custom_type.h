
#ifndef TOOLMAN_CUSTOM_TYPE_H
#define TOOLMAN_CUSTOM_TYPE_H

#include <vector>
#include <optional>

#include "type.h"
#include "struct_field.h"
#include "enum_field.h"

namespace toolman {

    template<typename F>
    class CustomType : public Type {
    public:
        using Type::Type;

        bool append_field(F f) {
            if (get_field_by_name(f.get_name()).has_value()) {
                return false;
            }
            fields_.push_back(f);
            return true;
        }

        std::optional<F> get_field_by_name(const std::string &field_name) const {
            for (const auto &f : fields_) {
                if (f.get_name() == field_name) {
                    return std::make_optional(f);
                }
            }
            return std::nullopt;
        }

    private:
        std::vector<F> fields_;
    };

    class StructType : public CustomType<StructField> {
        bool is_struct() const override { return true; }
    };

    class EnumType : public CustomType<EnumField> {
        bool is_enum() const override { return true; }
    };

}


#endif //TOOLMAN_CUSTOM_TYPE_H
