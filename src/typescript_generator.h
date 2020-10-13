// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_TYPESCRIPT_GENERATOR_H_
#define TOOLMAN_TYPESCRIPT_GENERATOR_H_

#include <memory>

#include "src/generator.h"

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
    ostream << "interface " << struct_type->get_name() << " {" << NL;
    for (const auto& field : struct_type->get_fields()) {
      ostream << field.get_name() << ": " << type_to_ts_type(field.get_type())
              << ";" << NL;
    }
    ostream << "}" << NL;
  }

  void generate_enum(std::ostream& ostream,
                     const std::shared_ptr<EnumType>& enum_type) const {
    ostream << "enum " << enum_type->get_name() << " {" << NL;
    for (const auto& field : enum_type->get_fields()) {
      ostream << field.get_name() << "=" << field.get_value() << "," << NL;
    }
    ostream << "}" << NL;
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
