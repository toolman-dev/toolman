// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#include "src/literal.h"

#include <utility>

#include "src/error.h"

namespace toolman {

template <typename P>
void MapLiteral::insert(P&& kv_pair) {
  value_.emplace(kv_pair);
}

template <typename L>
void ListLiteral::insert(L&& value_literal) {
  value_.emplace(std::forward<L>(value_literal));
}
}  // namespace toolman
