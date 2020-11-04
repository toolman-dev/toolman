// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_API_H_
#define TOOLMAN_API_H_

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
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

  Api(HttpMethod http_method, std::shared_ptr<Type> body_param)
      : http_method_(http_method), body_param_(std::move(body_param)) {}

  void add_path_param(PathParam path_param) {
    path_params_.push_back(std::move(path_param));
  }

  [[nodiscard]] std::optional<PathParam> get_path_param_by_name(
      const std::string& param_name) const {
    for (const auto& pp : path_params_) {
      if (pp.field.get_name() == param_name) {
        return std::make_optional(pp);
      }
    }
    return std::nullopt;
  }

  void set_path(std::string path) { path_ = std::move(path); }

 private:
  HttpMethod http_method_;
  std::string path_;
  std::vector<PathParam> path_params_;
  std::shared_ptr<Type> body_param_;

  std::vector<ApiReturn> returns_;
};

class ApiGroup {
 public:
  explicit ApiGroup(std::string group_name)
      : group_name_(std::move(group_name)) {}

  void add_api(Api api) { apis_.push_back(std::move(api)); }

 private:
  std::string group_name_;
  std::string api_prefix_;
  std::vector<Api> apis_;
};

}  // namespace toolman
#endif  // TOOLMAN_API_H_
