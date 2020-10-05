#ifndef STRUCT_TYPE_H
#define STRUCT_TYPE_H

#include <vector>
#include <optional>

#include "type.h"
#include "struct_field.h"

namespace toolman {

class StructType : public Type {
    public:
        
        using Type::Type;

        bool is_struct() const override { return true; }

        bool append(StructField f) {
            if (!get_field_by_name(f.get_name())) {
                return false;
            }
            fields.push_back(f);
            return true;
        }

        std::optional<const StructField> get_field_by_name(const std::string& field_name) const {
            for (const auto &f : fields) {
                if (f.get_name() == field_name) {
                    return std::make_optional(f);
                }
            }
            return std::nullopt;
        }
        
    private:
        std::vector<StructField> fields;

};

} // namespace toolman

#endif // STRUCT_TYPE_H
