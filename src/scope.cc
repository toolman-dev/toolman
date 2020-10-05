#include <optional>
#include <memory>
#include "type.h"
#include "scope.h"

namespace toolman {


    std::optional <std::shared_ptr<Type>> Scope::lookup_type(const std::string &name) {
        if (const auto it = types_.find(name); types_.end() != it) {
            return {it->second};
        }
        return std::nullopt;
    }

    std::optional <std::shared_ptr<Type>> Scope::declare(std::shared_ptr <Type> type) {
        if (auto emp_res = types_.emplace(type->get_name(), type); emp_res.second) {
            return {emp_res.first->second};
        }
        return std::nullopt;
    }
} // namespace toolman