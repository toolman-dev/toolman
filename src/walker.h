// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_WALKER_H_
#define TOOLMAN_WALKER_H_

#include "ToolmanParserBaseListener.h"

namespace toolman {

class DeclPhaseWalker final : public ToolmanParserBaseListener {};

class RefPhaseWalker final : public ToolmanParserBaseListener {};

}  // namespace toolman

#endif  // TOOLMAN_WALKER_H_
