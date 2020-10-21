// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_GOLANG_GENERATOR_H_
#define TOOLMAN_GOLANG_GENERATOR_H_

#include <memory>
#include <sstream>
#include <string>

#include "src/generator.h"
#include "src/list_type.h"
#include "src/map_type.h"
#include "src/primitive_type.h"

namespace toolman::generator {
class GolangGenerator : public Generator {
 protected:
  void before_generate_struct(std::ostream& ostream,
                              const Document* document) override {
    for (const auto& struct_type : document->get_struct_types()) {
      auto capitalized_struct_name = capitalize(struct_type->get_name());
      for (const auto& field : struct_type->get_fields()) {
        if (field.get_type()->is_oneof()) {
          auto oneof_name =
              gen_oneof_name(struct_type->get_name(), field.get_name());
          ostream << "type " << oneof_name << " interface {" << NL << INDENT_1
                  << oneof_name << "()" << NL << "}" << NL;
          auto oneof = std::dynamic_pointer_cast<OneofType>(field.get_type());
          for (const auto& oneof_field : oneof->get_fields()) {
            auto capitalized_field_name = capitalize(oneof_field.get_name());
            auto struct_name = capitalized_struct_name + capitalized_field_name;
            ostream << struct_name << " struct {" << NL << INDENT_1
                    << capitalized_field_name << " "
                    << type_to_go_type(oneof_field.get_type().get()) << NL
                    << "}" << NL2 << "func (*" << struct_name << ") "
                    << oneof_name << "() {}" << NL2;
          }
        }
      }
    }
    ostream << "type (" << NL;
  }

  void after_generate_struct(std::ostream& ostream,
                             const Document* document) override {
    ostream << ")" << NL2;
  }

  void after_generate_enum(std::ostream& ostream,
                           const Document* document) override {
    ostream << NL;
  }

  [[nodiscard]] std::string single_line_comment(
      std::string code) const override {
    return "// " + code;
  }

  void generate_struct(
      std::ostream& ostream,
      const std::shared_ptr<StructType>& struct_type) override {
    auto capitalized_struct_name = capitalize(struct_type->get_name());
    for (const auto& field : struct_type->get_fields()) {
      if (field.get_type()->is_oneof()) {
        auto oneof_name =
            gen_oneof_name(struct_type->get_name(), field.get_name());
        ostream << oneof_name + " interface {" << NL << INDENT_1 << oneof_name
                << "()" << NL << "}" << NL;
        auto oneof = std::dynamic_pointer_cast<OneofType>(field.get_type());
        for (const auto& oneof_field : oneof->get_fields()) {
          auto capitalized_field_name = capitalize(oneof_field.get_name());
          ostream << capitalized_struct_name << "_" << capitalized_field_name
                  << " struct {" << NL << INDENT_1 << capitalized_field_name
                  << " " << type_to_go_type(oneof_field.get_type().get()) << NL
                  << "}" << NL;
        }
      }
    }

    ostream << capitalized_struct_name << " struct {" << NL;
    for (const auto& field : struct_type->get_fields()) {
      ostream << "  " << capitalize(field.get_name()) << " "
              << (field.is_optional() && !field.get_type()->is_map() &&
                          !field.get_type()->is_list()
                      ? "*"
                      : "")
              << (field.get_type()->is_oneof()
                      ? gen_oneof_name(struct_type->get_name(),
                                       field.get_name())
                      : type_to_go_type(field.get_type().get()))
              << " `json:\"" + field.get_name() + "\"`" << NL;
    }
    ostream << "}" << NL;
  }

  void generate_enum(std::ostream& ostream,
                     const std::shared_ptr<EnumType>& enum_type) override {
    auto capitalized_name = capitalize(enum_type->get_name());
    ostream << "type " << capitalized_name << " int32" << NL;
    ostream << "const (" << NL;
    for (const auto& field : enum_type->get_fields()) {
      ostream << capitalized_name << "_" << field.get_name() << " "
              << capitalized_name << " = " << field.get_value() << NL;
    }
    ostream << ")" << NL;
  }

 private:
  static std::string gen_oneof_name(const std::string& struct_name,
                                    const std::string& field_name) {
    return "is" + capitalize(struct_name) + "_" + capitalize(field_name);
  }

  [[nodiscard]] static std::string type_to_go_type(Type* type) {
    if (type->is_primitive()) {
      auto primitive = dynamic_cast<PrimitiveType*>(type);
      if (primitive->is_bool()) {
        return "bool";
      } else if (primitive->is_i32()) {
        return "int32";
      } else if (primitive->is_u32()) {
        return "uint32";
      } else if (primitive->is_i64()) {
        return "int64";
      } else if (primitive->is_u64()) {
        return "uint64";
      } else if (primitive->is_float()) {
        return "float64";
      } else if (primitive->is_string()) {
        return "string";
      } else if (primitive->is_any()) {
        return "interface{}";
      }
    } else if (type->is_struct() || type->is_enum()) {
      return capitalize(type->get_name());
    } else if (type->is_list()) {
      auto list = dynamic_cast<ListType*>(type);
      return "[]" + type_to_go_type(list->get_elem_type().get());
    } else if (type->is_map()) {
      auto map = dynamic_cast<MapType*>(type);
      return "map[" + type_to_go_type(map->get_key_type().get()) + "]" +
             type_to_go_type(map->get_value_type().get());
    }
    return "";
  }
};
}  // namespace toolman::generator

#endif  // TOOLMAN_GOLANG_GENERATOR_H_
