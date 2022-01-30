# pragma once
# include <core/core.hpp>
# include <cstdio>
# include <cstdint>

namespace idascm
{
    // 0x07
    enum class game : std::uint8_t
    {
        unknown     = 0x00,
        gta3        = 0x01, // GTA III
        gtavc       = 0x02, // Vice City
        gtasa       = 0x03, // San Andreas
        gtalcs      = 0x04, // Liberty City Stories
        gtavcs      = 0x05, // Vice City Stories
    };
    auto to_string(game game) noexcept -> char const *;

    enum class edition : std::uint8_t
    {
        unknown             = 0x00,

        ps2                 = 0x01,
        ps2_ex              = 0x02,
        ps2_jpn             = 0x03,
        ps2_jpn_ex          = 0x04,
        ps2_ger             = 0x05,
        ps2_ger_ex          = 0x06,
        xbox                = 0x07,
        xbox_ex             = 0x08,
        pc                  = 0x0a,
        pc_ex               = 0x0b,
        pc_cleo             = 0x0c,
        psp                 = 0x0d,
        psp_ex              = 0x0e,

        anniversary         = 0x10,
        anniversary_cleo    = 0x11,
        definitive          = 0x14,
        definitive_cleo     = 0x15,

        custom              = 0x1f,
    };

    constexpr auto combine(game game, edition edition) noexcept -> std::uint8_t
    {
        return (to_uint(game) << 5) | to_uint(edition);
    }
    
    // TODO: check out definitive edition shit
    // TODO: check out re3 project
    enum class version : std::uint8_t
    {
        unknown             = combine(game::unknown, edition::unknown),

        gta3                = combine(game::gta3,   edition::unknown),      // GTA III
        gta3_ps2            = combine(game::gta3,   edition::ps2),          // GTA III (PS2) - 1.40
        gta3_ps2_ex         = combine(game::gta3,   edition::ps2_ex),       // GTA III (PS2) - 1.60
        gta3_xbox           = combine(game::gta3,   edition::xbox),         // GTA III (XBOX) - ?
        gta3_pc             = combine(game::gta3,   edition::pc),           // GTA III (PC) - 1.0
        gta3_pc_ex          = combine(game::gta3,   edition::pc_ex),        // GTA III (PC) - 1.1
        gta3_pc_cleo        = combine(game::gta3,   edition::pc_cleo),      // GTA III (PC with CLEO)
        gta3_anniversary    = combine(game::gta3,   edition::anniversary),  // GTA III - Anniversary Edition 2011
        gta3_definitive     = combine(game::gta3,   edition::definitive),   // GTA III - Defective Edition 2021
        gta3_custom         = combine(game::gta3,   edition::custom),       // GTA III - User Defined

        gtavc               = combine(game::gtavc,  edition::unknown),      // GTA: Vice City
        gtavc_ps2           = combine(game::gtavc,  edition::ps2),          // GTA: Vice City (PS2) - Initial Release
        gtavc_ps2_jpn       = combine(game::gtavc,  edition::ps2_jpn),      // GTA: Vice City (PS2) - Japanese Edition
        gtavc_ps2_ger       = combine(game::gtavc,  edition::ps2_ger),      // GTA: Vice City (PS2) - German Edition
        gtavc_xbox          = combine(game::gtavc,  edition::xbox),         // GTA: Vice City (XBOX) - Initial Release
        gtavc_pc            = combine(game::gtavc,  edition::pc),           // GTA: Vice City (PC) - Initial Release
        gtavc_pc_cleo       = combine(game::gtavc,  edition::pc_cleo),      // GTA: Vice City (PC with CLEO)
        gtavc_anniversary   = combine(game::gtavc,  edition::anniversary),  // GTA: Vice City - Anniversary Edition 2012
        gtavc_definitive    = combine(game::gtavc,  edition::definitive),   // GTA: Vice City - Defective Edition 2021
        gtavc_custom        = combine(game::gtavc,  edition::custom),       // GTA: Vice City - User Defined

        gtasa               = combine(game::gtasa,  edition::unknown),      // GTA: San Andreas
        gtasa_ps2           = combine(game::gtasa,  edition::ps2),          // GTA: San Andreas (PS2) - Initial Release 2004
        gtasa_xbox          = combine(game::gtasa,  edition::xbox),         // GTA: San Andreas (XBOX) - 2005
        gtasa_pc            = combine(game::gtasa,  edition::pc),           // GTA: San Andreas (PC) - Version 1.0 2005
        gtasa_pc_ex         = combine(game::gtasa,  edition::pc_ex),        // GTA: San Andreas (PC) - Version 2.0
        gtasa_pc_cleo       = combine(game::gtasa,  edition::pc_cleo),      // GTA: San Andreas (PC with CLEO)
        gtasa_anniversary   = combine(game::gtasa,  edition::anniversary),  // GTA: San Andreas - Anniversary Edition 2013
        gtasa_definitive    = combine(game::gtasa,  edition::definitive),   // GTA: San Andreas - Defective Edition 2021
        gtasa_custom        = combine(game::gtasa,  edition::custom),       // GTA: San Andreas - User Defined

        gtalcs              = combine(game::gtalcs, edition::unknown),      // GTA: Liberty City Stories
        gtalcs_psp          = combine(game::gtalcs, edition::psp),          // GTA: Liberty City Stories (PSP) - Initial Release 2005
        gtalcs_ps2          = combine(game::gtalcs, edition::ps2),          // GTA: Liberty City Stories (PS2) - Extended Version 2006
        gtalcs_anniversary  = combine(game::gtalcs, edition::anniversary),  // GTA: Liberty City Stories - Anniversary Edition 2015
        gtalcs_custom       = combine(game::gtalcs, edition::custom),       // GTA: Liberty City Stories - User Defined

        gtavcs              = combine(game::gtavcs, edition::unknown),      // GTA: Vice City Stories
        gtavcs_psp          = combine(game::gtavcs, edition::psp),          // GTA: Vice City Stories (PSP) - Initial Release 2006
        gtavcs_ps2          = combine(game::gtavcs, edition::ps2),          // GTA: Vice City Stories (PS2) - Extended Version 2007
        gtavcs_custom       = combine(game::gtavcs, edition::custom),       // GTA: Vice City Stories - User Defined
    };

    auto to_string(version version) noexcept -> char const *;
    auto to_version(char const * string) noexcept -> version;

    constexpr auto version_game(version ver) noexcept -> game
    {
        return static_cast<game>(to_uint(ver) >> 5);
    }
    constexpr auto version_edition(version ver) noexcept -> edition
    {
        return static_cast<edition>(to_uint(ver) & 0x1f);
    }
    auto version_description(version ver) noexcept -> char const *;
}
