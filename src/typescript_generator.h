// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_TYPESCRIPT_GENERATOR_H_
#define TOOLMAN_TYPESCRIPT_GENERATOR_H_

#include <memory>
#include <string>
#include <vector>

#include "src/field.h"
#include "src/generator.h"
#include "src/list_type.h"
#include "src/map_type.h"
#include "src/primitive_type.h"
#include "src/type.h"

namespace toolman {
class TypescriptGenerator : public Generator {
 protected:
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
    ostream << "export interface " << struct_type->get_name() << " {" << NL;
    for (const auto& field : struct_type->get_fields()) {
      ostream << INDENT_1;
      generate_field(ostream, &field);
      ostream << NL;
    }
    ostream << "}" << NL;
  }

  void generate_enum(std::ostream& ostream,
                     const std::shared_ptr<EnumType>& enum_type) override {
    ostream << "export enum " << enum_type->get_name() << " {" << NL;
    for (const auto& field : enum_type->get_fields()) {
      ostream << INDENT_1 << field.get_name() << "=" << field.get_value() << ","
              << NL;
    }
    ostream << "}" << NL;
  }

 private:
  void generate_field(std::ostream& ostream, const Field* field) const {
    ostream << field->get_name();
    if (field->is_optional()) {
      ostream << "?";
    }
    ostream << ": ";
    if (field->get_type()->is_oneof()) {
      auto oneof = std::dynamic_pointer_cast<OneofType>(field->get_type());
      auto oneof_fields = oneof->get_fields();
      for (auto it = oneof_fields.begin(); it != oneof_fields.end(); ++it) {
        ostream << "{ ";
        generate_field(ostream, &(*it));
        ostream << " }";
        if (it != (oneof_fields.end() - 1)) {
          ostream << " | ";
        }
      }
    } else {
      ostream << type_to_ts_type(field->get_type().get());
    }
    ostream << ";";
  }

  [[nodiscard]] static std::string type_to_ts_type(Type* type) {
    if (type->is_primitive()) {
      auto primitive = dynamic_cast<PrimitiveType*>(type);
      if (primitive->is_bool()) {
        return "boolean";
      } else if (primitive->is_numeric()) {
        return "number";
      } else if (primitive->is_string()) {
        return "string";
      } else if (primitive->is_any()) {
        return "any";
      }
    } else if (type->is_struct() || type->is_enum()) {
      return type->get_name();
    } else if (type->is_map()) {
      auto map = dynamic_cast<MapType*>(type);
      return "{[key: " + type_to_ts_type(map->get_key_type().get()) +
             "]: " + type_to_ts_type(map->get_value_type().get()) + ";}";
    } else if (type->is_list()) {
      auto list = dynamic_cast<ListType*>(type);
      return "{[index: number]: " +
             type_to_ts_type(list->get_elem_type().get()) + ";}";
    }
    return "";
  }
};
}  //  namespace toolman

#endif  // TOOLMAN_TYPESCRIPT_GENERATOR_H_
