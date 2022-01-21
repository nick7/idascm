# pragma once
# include <cstdint>
# include <type_traits>

# define IDASCM_STRINGIFY(X)    #X
# define IDASCM_TOSTRING(X)     IDASCM_STRINGIFY(X)

# define IDASCM_VERSION_STRING  IDASCM_GIT_REVISION

# if defined _WIN32
#   define IDASCM_PLATFORM_WINDOWS
# endif

namespace idascm
{
    auto build_version(void) noexcept -> char const *;

    template <typename type>
    constexpr auto to_uint(type value) noexcept -> std::enable_if_t<std::is_enum_v<type>, std::underlying_type_t<type>>
    {
        return static_cast<std::underlying_type_t<type>>(value);
    }
}
