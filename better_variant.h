#pragma once

#include <optional>
#include <stdexcept>
#include <variant>
	
#if __cplusplus >= 202302L
namespace bv {

template<typename... Ts>
struct Match : Ts... {
  using Ts::operator()...;
};

#define MATCH(...) Match {__VA_ARGS__}

template<typename... Variants> 
class Variant {
public:
  template<typename T>
    requires(!std::same_as<std::decay_t<T>, Variant>)
  Variant(T&& value) : data(std::forward<T>(value)) {}

  template<typename T> 
  static Variant make(T&& value) {
    return Variant{std::forward<T>(value)};
  }

  template<typename T> 
  bool is() const {
    return std::holds_alternative<T>(data);
  }

  template<typename T> 
  std::optional<T> tryAs(this auto&& self) {
    if (!self.template is<T>()) {
      return std::nullopt;
    }

    return std::get<T>(self.data);
  }

  template<typename T> 
  T as(this auto&& self) {
    if (!self.template is<T>()) {
      throw std::runtime_error("Invalid variant access");
    }

    return std::get<T>(self.data);
  }

  template<typename Self, typename... Visitors>
  auto match(this Self&& self, Visitors&& ...visitors) {
    return std::visit(Match{std::forward<Visitors>(visitors)...},
                      std::forward<Self>(self).data);
  }

  template<typename Self, typename... Visitors>
  auto operator|(this Self&& self, Match<Visitors...> match) {
    return std::visit(match, std::forward<Self>(self).data);
  }

private:
  std::variant<Variants...> data;
};

} // namespace bv

#else
static_assert(__cplusplus >= 202302L, "This library requires C++23 or greater.");
#endif