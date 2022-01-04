# pragma once
# include <cstdint>

# define IDASCM_STRINGIFY(X)    #X
# define IDASCM_TOSTRING(X)     IDASCM_STRINGIFY(X)

# define IDASCM_VERSION_STRING  IDASCM_GIT_REVISION

# if defined _WIN32
#   define IDASCM_PLATFORM_WINDOWS
# endif

namespace idascm
{
    auto build_version(void) noexcept -> char const *;
}
