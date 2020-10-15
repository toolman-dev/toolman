// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_TYPESCRIPT_GENERATOR_H_
#define TOOLMAN_TYPESCRIPT_GENERATOR_H_

#include <memory>
#include <vector>

#include "src/field.h"
#include "src/generator.h"
#include "src/primitive_type.h"

namespace toolman {
class TypescriptGenerator : public Generator {
 public:
  void before_generate_struct(std::ostream& ostream,
                              const Document* document) const {}
  void after_generate_struct(std::ostream& ostream,
                             const Document* document) const {}
  void before_generate_enum(std::ostream& ostream,
                            const Document* document) const {}
  void after_generate_enum(std::ostream& ostream,
                           const Document* document) const {
    ostream << NL;
  }

  void generate_struct(std::ostream& ostream,
                       const std::shared_ptr<StructType>& struct_type) const {
    ostream << "export interface " << struct_type->get_name() << " {" << NL;
    for (const auto& field : struct_type->get_fields()) {
      generate_field(ostream, &field);
      ostream << NL;
    }
    ostream << "}" << NL;
  }

  void generate_enum(std::ostream& ostream,
                     const std::shared_ptr<EnumType>& enum_type) const {
    ostream << "export enum " << enum_type->get_name() << " {" << NL;
    for (const auto& field : enum_type->get_fields()) {
      ostream << field.get_name() << "=" << field.get_value() << "," << NL;
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
      for (std::vector<Field>::iterator it = oneof_fields.begin();
           it != oneof_fields.end(); ++it) {
        ostream << "{ ";
        generate_field(ostream, &(*it));
        ostream << " }";
        if (it != (oneof_fields.end() - 1)) {
          ostream << " | ";
        }
      }
    } else {
      ostream << type_to_ts_type(field->get_type());
    }
    ostream << ";";
  }

  [[nodiscard]] static std::string type_to_ts_type(
      const std::shared_ptr<Type>& type) {
    if (type->is_primitive()) {
      auto primitive = std::dynamic_pointer_cast<PrimitiveType>(type);
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
      auto map = std::dynamic_pointer_cast<MapType>(type);
      return "{[key: " + type_to_ts_type(map->get_key_type()) +
             "]: " + type_to_ts_type(map->get_value_type()) + ";}";
    } else if (type->is_list()) {
      auto list = std::dynamic_pointer_cast<ListType>(type);
      return "{[index: number]: " + type_to_ts_type(list->get_elem_type()) +
             ";}";
    }
    return "";
  }
};
}  //  namespace toolman

#endif  // TOOLMAN_TYPESCRIPT_GENERATOR_H_
