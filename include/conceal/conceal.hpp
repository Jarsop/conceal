#pragma once

#include <concepts>
#include <iostream>
#include <utility>

#ifdef CONCEAL_USE_JSON
#include <nlohmann/json.hpp>
#endif

namespace conceal {

#ifndef CONCEAL_HIDDEN_MSG
#define CONCEAL_HIDDEN_MSG "<hidden>"
#endif

template <typename T> class Conceal {
public:
  // Constructors
  Conceal() : value_{} {}
  explicit Conceal(const T &val) : value_(val) {}
  explicit Conceal(T &&val) : value_{std::forward<T>(val)} {}

  template <typename U>
    requires std::convertible_to<U, T>
  Conceal(const Conceal<U> &other) : value_(*other) {}

  template <typename U>
    requires std::convertible_to<U, T>
  Conceal(Conceal<U> &&other) : value_{std::move(*other)} {
    if constexpr (std::is_same_v<T, U>) {
      other.value_ = T{};
    } else {
      other = Conceal<U>{};
    }
  }

  template <typename U>
    requires std::constructible_from<T, std::initializer_list<U>>
  Conceal(std::initializer_list<U> il) : value_(il) {}

  // Assignment
  Conceal(const Conceal &) = default;
  Conceal &operator=(const Conceal &) = default;

  Conceal(Conceal &&other) noexcept : value_(std::move(other.value_)) {
    other.value_ = T{};
  }
  Conceal &operator=(Conceal &&other) noexcept {
    if (this != &other) {
      value_ = std::move(other.value_);
      other.value_ = T{};
    }
    return *this;
  }

  template <typename U>
    requires std::convertible_to<U, T>
  Conceal &operator=(const U &val) {
    value_ = val;
    return *this;
  }
  template <typename U>
    requires std::convertible_to<U, T>
  Conceal &operator=(U &&val) {
    value_ = std::forward<U>(val);
    return *this;
  }

  // Access and conversion
  operator const T &() const { return value_; }
  operator T &() { return value_; }
  operator const T &&() const && { return std::move(value_); }
  operator T &&() && { return std::move(value_); }
  operator const T *() const { return &value_; }
  operator T *() { return &value_; }
  const T *operator->() const { return &value_; }
  T *operator->() { return &value_; }

  // Comparisons
  bool operator==(const Conceal &other) const { return value_ == other.value_; }
  bool operator!=(const Conceal &other) const { return !(*this == other); }
  bool operator<(const Conceal &other) const { return value_ < other.value_; }
  bool operator<=(const Conceal &other) const { return value_ <= other.value_; }
  bool operator>(const Conceal &other) const { return value_ > other.value_; }
  bool operator>=(const Conceal &other) const { return value_ >= other.value_; }

  template <typename U>
    requires std::convertible_to<U, T>
  bool operator==(const U &other) const {
    return value_ == other;
  }

  // Output and serialization
  friend std::ostream &operator<<(std::ostream &os, const Conceal &) {
    return os << CONCEAL_HIDDEN_MSG;
  }

#ifdef CONCEAL_USE_JSON
  friend void to_json(nlohmann::json &j, const Conceal &) { j = nullptr; }

  friend void from_json(const nlohmann::json &j, Conceal &obj) {
    obj.value_ = T{};
    if (!j.is_null()) {
      j.get_to(obj.value_);
    }
  }
#endif // CONCEAL_USE_JSON

private:
  T value_;
};

} // namespace conceal
