# include <engine/version.hpp>
# include <algorithm>
# include <string_view>

namespace idascm
{
    namespace
    {
        char const * const l_game_table[] = \
        {
            /* [unknown]   = */     "unknown",
            /* [gta3]      = */     "gta3",
            /* [gtavc]     = */     "gtavc",
            /* [gtasa]     = */     "gtasa",
            /* [gtalcs]    = */     "gtalcs",
            /* [gtavcs]    = */     "gtavcs",
        };
    }

    auto to_string(game game) noexcept -> char const *
    {
        if (static_cast<std::uint8_t>(game) < std::size(l_game_table))
            return l_game_table[static_cast<std::uint8_t>(game)];
        return nullptr;
    }

    auto to_string(platform platform) noexcept -> char const *
    {
        switch (platform)
        {
            case platform::ps2:
                return "ps2";
            case platform::xbox:
                return "xbox";
            case platform::win32:
                return "win32";
            case platform::psp:
                return "psp";
            case platform::ps3:
                return "ps3";
            case platform::xbox360:
                return "xbox360";
            case platform::android:
                return "android";
            default:
                break;
        }
        return nullptr;
    }

    namespace
    {
        struct
        {
            version         value;
            char const *    string;
        }
        const g_version_table[] = \
        {
            { version::gta3_ps2,            "gta3_ps2"          },
            { version::gta3_win32,          "gta3_win32"        },
            { version::gta3_xbox,           "gta3_xbox"         },
            { version::gta3_android,        "gta3_android"      },
            { version::gtavc_ps2,           "gtavc_ps2"         },
            { version::gtavc_win32,         "gtavc_win32"       },
            { version::gtavc_xbox,          "gtavc_xbox"        },
            { version::gtavc_xbox_jp,       "gtavc_xbox_jp"     },
            { version::gtalcs_ps2,          "gtalcs_ps2"        },
            { version::gtalcs_psp,          "gtalcs_psp"        },
            { version::gtavcs_ps2,          "gtavcs_ps2"        },
            { version::gtavcs_psp,          "gtavcs_psp"        },
        };
    }

    auto to_string(version version) noexcept -> char const *
    {
        for (auto const & row : g_version_table)
        {
            if (version == row.value)
                return row.string;
        }
        return nullptr;
    }

    auto to_version(char const * string) noexcept -> version
    {
        if (string && string[0])
        {
            for (auto const & row : g_version_table)
            {
                if (0 == std::strcmp(string, row.string))
                    return row.value;
            }
        }
        return version::unknown;
    }
}
