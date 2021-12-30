# pragma once
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

    // 0x38
    enum class platform : std::uint8_t
    {
        unknown     = 0x0 << 3,
        ps2         = 0x1 << 3, // Sony PlayStation 2
        xbox        = 0x2 << 3,
        win32       = 0x3 << 3, // Windows x86
        psp         = 0x4 << 3, // Sony PlayStation Portable
        ps3         = 0x5 << 3, // Sony PlayStation 3
        xbox360     = 0x6 << 3, // Microsoft XBOX 360
        android     = 0x7 << 3, // Android
    };
    // auto to_string(platform platform) noexcept -> char const *;

    enum class edition : std::uint8_t
    {
        basic       = 0x0 << 6,
        japanese    = 0x1 << 6,
    };

    constexpr auto operator | (uint8_t value, game game) -> std::uint8_t
    {
        return value | static_cast<std::uint8_t>(game);
    }

    constexpr auto operator | (uint8_t value, platform platform) -> std::uint8_t
    {
        return value | static_cast<std::uint8_t>(platform);
    }

    constexpr auto operator | (uint8_t value, edition edition) -> std::uint8_t
    {
        return value | static_cast<std::uint8_t>(edition);
    }

    constexpr auto combine(game game, platform platform, edition edition = edition::basic) -> std::uint8_t
    {
        return static_cast<std::uint8_t>(game) | static_cast<std::uint8_t>(platform) | static_cast<std::uint8_t>(edition);
    }
    
    // TODO: check out definitive edition shit
    enum class version : std::uint8_t
    {
        unknown         = combine(game::unknown,    platform::unknown),
        gta3_ps2        = combine(game::gta3,       platform::ps2,      edition::basic),
        gta3_win32      = combine(game::gta3,       platform::win32,    edition::basic),
        gta3_xbox       = combine(game::gta3,       platform::xbox,     edition::basic),
        gta3_android    = combine(game::gta3,       platform::android,  edition::basic),
        gtavc_ps2       = combine(game::gtavc,      platform::ps2,      edition::basic),
        gtavc_win32     = combine(game::gtavc,      platform::win32,    edition::basic),
        gtavc_xbox      = combine(game::gtavc,      platform::xbox,     edition::basic),
        gtavc_xbox_jp   = combine(game::gtavc,      platform::xbox,     edition::japanese),
        gtavc_android   = combine(game::gtavc,      platform::android,  edition::basic),
        gtasa_ps2       = combine(game::gtasa,      platform::ps2,      edition::basic),
        gtasa_win32     = combine(game::gtasa,      platform::win32,    edition::basic),
        gtasa_xbox      = combine(game::gtasa,      platform::xbox,     edition::basic),
        gtasa_android   = combine(game::gtasa,      platform::android,  edition::basic),
        gtalcs_ps2      = combine(game::gtalcs,     platform::ps2,      edition::basic),
        gtalcs_psp      = combine(game::gtalcs,     platform::psp,      edition::basic),
        gtalcs_android  = combine(game::gtalcs,     platform::android,  edition::basic),
        gtavcs_ps2      = combine(game::gtavcs,     platform::ps2,      edition::basic),
        gtavcs_psp      = combine(game::gtavcs,     platform::psp,      edition::basic),
    };

    constexpr auto to_uint(version ver) -> std::uint8_t
    {
        return static_cast<std::uint8_t>(ver);
    }

    constexpr auto version_from_uint(std::uint8_t value) -> version
    {
        return static_cast<version>(value);
    }

    auto to_string(version version) noexcept -> char const *;
    auto to_version(char const * string) noexcept -> version;
}
