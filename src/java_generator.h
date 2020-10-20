// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_JAVA_GENERATOR_H_
#define TOOLMAN_JAVA_GENERATOR_H_

#include <memory>
#include <sstream>
#include <string>

#include "src/field.h"
#include "src/generator.h"
#include "src/list_type.h"
#include "src/map_type.h"
#include "src/primitive_type.h"
#include "src/scope.h"

namespace toolman {
class JavaGenerator : public Generator {
 protected:
  void before_generate_document(std::ostream &ostream,
                                const Document *document) override {
    // process option
    for (const auto &opt : document->get_options()) {
      if (opt->get_name() == buildin::option_use_java8_optional.get_name()) {
        auto bool_opt = std::dynamic_pointer_cast<decltype(
            buildin::option_use_java8_optional)>(opt);
        use_java8_optional_ = bool_opt->get_value();
      }
    }

    auto outclass =
        capitalize(camelcase(document->get_source()->stem().string()));
    ostream << "public final class " << outclass << " {" << NL << INDENT_1
            << "private " << outclass << "() {}" << NL;
  }
  void after_generate_document(std::ostream &ostream,
                               const Document *document) override {
    ostream << NL << "}" << NL;
  }

  void before_generate_struct(std::ostream &ostream,
                              const Document *document) override {
    for (const auto &struct_type : document->get_struct_types()) {
      auto struct_name = capitalize(camelcase(struct_type->get_name()));
      for (const auto &field : struct_type->get_fields()) {
        if (field.get_type()->is_oneof()) {
          auto oneof_name =
              gen_oneof_name(struct_type->get_name(), field.get_name());
          ostream << INDENT_1 << "public interface " << oneof_name << " {}"
                  << NL;
          auto oneof = std::dynamic_pointer_cast<OneofType>(field.get_type());
          for (const auto &oneof_field : oneof->get_fields()) {
            auto field_name = camelcase(oneof_field.get_name());
            auto oneof_item_class_name = struct_name + capitalize(field_name);
            ostream << INDENT_1 << "public class " << oneof_item_class_name
                    << " implements " << oneof_name << " {" << NL << INDENT_2
                    << generate_struct_field(struct_type.get(), oneof_field)
                    << NL
                    << generate_getter_and_setter(struct_type.get(),
                                                  oneof_field, INDENT_2)
                    << INDENT_1 << "}" << NL;
          }
        }
      }
    }
  }

  [[nodiscard]] std::string single_line_comment(
      std::string code) const override {
    return "// " + code;
  }

  void generate_struct(
      std::ostream &ostream,
      const std::shared_ptr<StructType> &struct_type) override {
    ostream << INDENT_1 << "public static final class "
            << struct_type->get_name() << " implements java.io.Serializable {"
            << NL << INDENT_2
            << "private static final long serialVersionUID = 0L;" << NL;

    for (const auto &field : struct_type->get_fields()) {
      ostream << INDENT_2 << generate_struct_field(struct_type.get(), field)
              << NL;
    }
    ostream << NL;

    for (const auto &field : struct_type->get_fields()) {
      ostream << generate_getter_and_setter(struct_type.get(), field, INDENT_2);
    }

    ostream << INDENT_1 << "}" << NL2;
  }

  void generate_enum(std::ostream &ostream,
                     const std::shared_ptr<EnumType> &enum_type) override {
    ostream << INDENT_1 << "public enum " << enum_type->get_name() << " {"
            << NL;

    for (const auto &field : enum_type->get_fields()) {
      ostream << INDENT_2 << camelcase(field.get_name()) << "("
              << field.get_value() << ")," << NL;
    }
    ostream << INDENT_2 << ";" << NL;

    ostream << INDENT_2 << "public static "
            << (use_java8_optional_ ? "java.util.Optional<" : "")
            << enum_type->get_name() << (use_java8_optional_ ? ">" : "")
            << " forNumber(int value) {" << NL << INDENT_3 << "switch (value) {"
            << NL;
    for (const auto &field : enum_type->get_fields()) {
      ostream << INDENT_4 << "case " << field.get_value() << ": return "
              << (use_java8_optional_ ? "java.util.Optional.of(" : "")
              << camelcase(field.get_name())
              << (use_java8_optional_ ? ");" : ";") << NL;
    }
    ostream << INDENT_4 << "default: return "
            << (use_java8_optional_ ? "java.util.Optional.empty();" : "null;")
            << NL << INDENT_3 << "}" << NL << INDENT_2 << "}" << NL;

    ostream << INDENT_2 << "private final int value;" << NL << INDENT_2
            << "private " + enum_type->get_name() + "(int value) {" << NL
            << INDENT_3 << "this.value = value;" << NL << INDENT_2 << "}" << NL
            << INDENT_1 << "}";
  }

 private:
  std::string generate_struct_field(StructType *struct_type,
                                    const Field &field) const {
    auto use_optional = use_java8_optional_ && field.is_optional();
    return (use_optional ? "private java.util.Optional<" : "private ") +
           (field.get_type()->is_oneof()
                ? gen_oneof_name(struct_type->get_name(), field.get_name())
                : type_to_java_type(field.get_type().get(),
                                    field.is_optional())) +
           (use_optional ? "> " : " ") + field.get_name() + ";";
  }

  std::string generate_getter_and_setter(StructType *struct_type,
                                         const Field &field,
                                         const std::string &base_indent) const {
    auto use_optional = use_java8_optional_ && field.is_optional();
    auto field_name_camelcase = camelcase(field.get_name());
    // getter
    auto getter =
        base_indent +
        (use_optional ? "public java.util.Optional<" : "public ") +
        type_to_java_type(field.get_type().get(), field.is_optional()) +
        (use_optional ? "> get" : " get") + capitalize(field_name_camelcase) +
        "() {" + NL + base_indent + INDENT_1 + "return " +
        field_name_camelcase + ";" + NL + base_indent + "}" + NL;

    // setter
    auto setter =
        base_indent + "public void set" + capitalize(field_name_camelcase) +
        "(" + (use_optional ? "java.util.Optional<" : "") +
        type_to_java_type(field.get_type().get(), field.is_optional()) + " " +
        field_name_camelcase + ") {" + NL + base_indent + INDENT_1 + "this." +
        field_name_camelcase + " = " + field_name_camelcase + ";" + NL +
        base_indent + "}";
    return getter + setter + NL;
  }

  static std::string gen_oneof_name(const std::string &struct_name,
                                    const std::string &field_name) {
    return "Is" + capitalize(camelcase(struct_name)) +
           capitalize(camelcase(field_name));
  }

  [[nodiscard]] static std::string type_to_java_type(Type *type,
                                                     bool boxed = false) {
    if (type->is_primitive()) {
      auto primitive = dynamic_cast<PrimitiveType *>(type);
      if (primitive->is_bool()) {
        return boxed ? "Boolean" : "bool";
      } else if (primitive->is_i32() || primitive->is_u32()) {
        return boxed ? "Integer" : "int";
      } else if (primitive->is_i64() || primitive->is_u64()) {
        return boxed ? "Long" : "long";
      } else if (primitive->is_float()) {
        return boxed ? "Float" : "float";
      } else if (primitive->is_string()) {
        return "String";
      } else if (primitive->is_any()) {
        return "Object";
      }
    } else if (type->is_struct() || type->is_enum()) {
      return type->get_name();
    } else if (type->is_list()) {
      auto list = dynamic_cast<ListType *>(type);
      return "java.util.List<" +
             type_to_java_type(list->get_elem_type().get(), true) + ">";
    } else if (type->is_map()) {
      auto map = dynamic_cast<MapType *>(type);
      return "java.util.Map<" +
             type_to_java_type(map->get_key_type().get(), true) + ", " +
             type_to_java_type(map->get_value_type().get(), true) + ">";
    }
    return "";
  }
  bool use_java8_optional_ = false;
};
}  // namespace toolman
#endif  // TOOLMAN_GOLANG_GENERATOR_H_
