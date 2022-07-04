#pragma once

// Its pure madness, C++ have deducing by return type(really, specialization without saying for what specialization is...)
// Please, don't code like that!
namespace madness {

  class cell {
   public:
    template <typename T>
    T value() const;
  };

  template <>
  float cell::value() const {
    return 3.14f;
  }

  template <>
  double cell::value() const {
    return 3.14;
  }

}  // namespace madness