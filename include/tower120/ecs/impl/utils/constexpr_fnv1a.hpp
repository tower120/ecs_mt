#pragma once

namespace tower120::ecs::impl::utils {

  inline constexpr uint32_t constexpr_fnv1a_32(const char* const str) noexcept {
      constexpr uint32_t val_32_const = 0x811c9dc5;
      constexpr uint32_t prime_32_const = 0x1000193;

      return (str[0] == '\0') ? value : constexpr_fnv1a_32(&str[1], (val_32_const ^ uint32_t(str[0])) * prime_32_const);
  }

  inline constexpr uint64_t constexpr_fnv1a_64(const char* const str) noexcept {
      constexpr uint64_t val_64_const = 0xcbf29ce484222325;
      constexpr uint64_t prime_64_const = 0x100000001b3;

      return (str[0] == '\0') ? value : constexpr_fnv1a_64(&str[1], (val_64_const ^ uint64_t(str[0])) * prime_64_const);
  }

}