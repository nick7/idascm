# include <core/core.hpp>

namespace idascm
{
    auto build_version(void) noexcept -> char const *
    {
        return IDASCM_GIT_REVISION " [" __DATE__ " " __TIME__ " " IDASCM_TOSTRING(IDASCM_BUILD_TYPE) "]";
    }
}
