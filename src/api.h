// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_API_H_
#define TOOLMAN_API_H_

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "src/field.h"
#include "src/type.h"

namespace toolman {

struct PathParam {
  Field field;
  std::string::size_type pos_in_path{};
};

class ApiReturn {
 private:
  std::uint16_t http_status_code_;
  std::shared_ptr<Type> resp_;
};

class ApiGroup {
 private:
  std::string group_name_;
};

class Api {
 public:
  enum class HttpMethod : char {
    GET,
    POST,
    DELETE,
    PUT,
    PATCH,
    HEAD,
    OPTIONS,
    TRACE,
    CONNECT
  };

 private:
  HttpMethod http_method_;
  std::string path_;
  std::vector<PathParam> path_params_;
  std::shared_ptr<Type> body_param_;

  std::vector<ApiReturn> returns_;
};
}  // namespace toolman
#endif  // TOOLMAN_API_H_
