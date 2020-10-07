// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#include "src/walker.h"

namespace toolman {

template <typename NODE, typename DECL_TYPE>
void DeclPhaseWalker::decl_type(NODE* node) {
  auto id_start_token = node->identifierName()->getStart();
  auto stmt_info = StmtInfo(
      {id_start_token->getLine(), node->identifierName()->getStop()->getLine()},
      {id_start_token->getStartIndex(), id_start_token->getStopIndex()}, file_);
  if (auto search = type_scope_.lookup_type(node->identifierName()->getText());
      search.has_value()) {
    errors_.emplace_back(DuplicateDeclError(search.value(), stmt_info));
    return;
  } else {
    type_scope_.declare(std::make_shared<DECL_TYPE>(DECL_TYPE(
        node->identifierName()->getText(), stmt_info, node->Pub() != nullptr)));
  }
}

}  // namespace toolman
