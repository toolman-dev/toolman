// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_CUSTOM_TYPE_H_
#define TOOLMAN_CUSTOM_TYPE_H_

#include <vector>
#include <optional>
#include <string>

#include "src/type.h"

namespace toolman {

class StructField;
class EnumField;

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

}  // namespace toolman


#endif  // TOOLMAN_CUSTOM_TYPE_H_
