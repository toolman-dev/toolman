// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#include "src/generator.h"

#include "src/document.h"
#include "src/golang_generator.h"
#include "src/java_generator.h"
#include "src/typescript_generator.h"

namespace toolman::generator {
void generate(std::unique_ptr<Document> document, TargetLanguage targetLanguage,
              std::ostream& ostream) {
  std::unique_ptr<Generator> generator;
  switch (targetLanguage) {
    case TargetLanguage::GOLANG:
      generator = std::make_unique<GolangGenerator>();
      break;
    case TargetLanguage::TYPESCRIPT:
      generator = std::make_unique<TypescriptGenerator>();
      break;
    case TargetLanguage::JAVA:
      generator = std::make_unique<JavaGenerator>();
      break;
  }
  generator->generate(ostream, document);
}

std::string underscore(std::string in) {
  in[0] = tolower(in[0]);
  for (size_t i = 1; i < in.size(); ++i) {
    if (isupper(in[i])) {
      in[i] = tolower(in[i]);
      in.insert(i, "_");
    }
  }
  return in;
}

std::string camelcase(const std::string& in) {
  std::ostringstream out;
  bool underscore = false;

  for (char c : in) {
    if (c == '_') {
      underscore = true;
      continue;
    }
    if (underscore) {
      out << (char)toupper(c);
      underscore = false;
      continue;
    }
    out << c;
  }

  return out.str();
}
std::string capitalize(std::string in) {
  in[0] = toupper(in[0]);
  return in;
}

std::string decapitalize(std::string in) {
  in[0] = tolower(in[0]);
  return in;
}
}  // namespace toolman::generator
