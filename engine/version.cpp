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

    namespace
    {
        struct
        {
            version         value;
            char const *    string;
            char const *    description;
        }
        const g_version_table[] = \
        {
            { version::gta3_ps2,            "gta3_ps2",             "GTA III (PlayStation 2)"                       },
            { version::gta3_ps2_ex,         "gta3_ps2_ex",          "GTA III (PlayStation 2 Extended)"              },
            { version::gta3_xbox,           "gta3_xbox",            "GTA III (XBOX Original)"                       },
            { version::gta3_pc,             "gta3_pc",              "GTA III (PC 1.0)"                              },
            { version::gta3_pc_ex,          "gta3_pc_ex",           "GTA III (PC 1.1)"                              },
            { version::gta3_anniversary,    "gta3_anniversary",     "GTA III (Anniversary Edition 2011)"            },
            { version::gta3_definitive,     "gta3_definitive",      "GTA III (Definitive Edition 2021)"             },

            { version::gtavc_ps2,           "gtavc_ps2",            "GTA Vice City (PlayStation 2)"                 },
            { version::gtavc_ps2_jpn,       "gtavc_ps2_jpn",        "GTA Vice City (PlayStation 2 Japanese)"        },
            { version::gtavc_ps2_ger,       "gtavc_ps2_ger",        "GTA Vice City (PlayStation 2 German)"          },
            { version::gtavc_xbox,          "gtavc_xbox",           "GTA Vice City (XBOX Original)"                 },
            { version::gtavc_pc,            "gtavc_pc",             "GTA Vice City (PC)"                            },
            { version::gtavc_anniversary,   "gtavc_anniversary",    "GTA Vice City (Anniversary Edition 2012)"      },
            { version::gtavc_definitive,    "gtavc_definitive",     "GTA Vice City (Definitive Edition 2021)"       },

            { version::gtasa_ps2,           "gtasa_ps2",            "GTA San Andreas (PlayStation 2)"               },
            { version::gtasa_xbox,          "gtasa_xbox",           "GTA San Andreas (XBOX Original)"               },
            { version::gtasa_pc,            "gtasa_pc",             "GTA San Andreas (PC)"                          },
            { version::gtasa_pc_cleo,       "gtasa_pc_cleo",        "GTA San Andreas (PC with CLEO)"                },
            { version::gtasa_anniversary,   "gtasa_anniversary",    "GTA San Andreas (Anniversary Edition 2013)"    },
            { version::gtasa_definitive,    "gtasa_definitive",     "GTA San Andreas (Definitive Edition 2021)"     },

            { version::gtalcs_ps2,          "gtalcs_ps2",           "GTA Librery City Stories (PlayStation 2)"              },
            { version::gtalcs_psp,          "gtalcs_psp",           "GTA Librery City Stories (PlayStation Portable)"       },
            { version::gtalcs_anniversary,  "gtalcs_anniversary",   "GTA Librery City Stories (Anniversary Edition 2015)"   },

            { version::gtavcs_ps2,          "gtavcs_ps2"            "GTA Vice City Stories (PlayStation 2)"                 },
            { version::gtavcs_psp,          "gtavcs_psp"            "GTA Vice City Stories (PlayStation Portable)"          },
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
    
    auto version_description(version ver) noexcept -> char const *
    {
        for (auto const & row : g_version_table)
        {
            if (ver == row.value)
                return row.description;
        }
        return nullptr;
    }
}
