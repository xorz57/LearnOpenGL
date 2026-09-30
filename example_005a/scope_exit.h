#pragma once

#include <utility>

template <typename Function> class ScopeExit final {
public:
  explicit ScopeExit(Function function) : function_{std::move(function)} {}
  ~ScopeExit() { function_(); }

  ScopeExit(const ScopeExit &) = delete;
  auto operator=(const ScopeExit &) -> ScopeExit & = delete;

  ScopeExit(ScopeExit &&) = delete;
  auto operator=(ScopeExit &&) -> ScopeExit & = delete;

private:
  Function function_;
};
