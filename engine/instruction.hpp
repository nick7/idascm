# pragma once
# include <engine/engine.hpp>
# include <engine/operand.hpp>
# include <string_view>

namespace idascm
{
    struct command;

    enum instruction_flag : std::uint8_t
    {
        instruction_flag_not    = 1 << 0, // condition inversion
    };

    // Command instance with actual runtime values
    struct instruction
    {
        command const *     command;
        std::uint32_t       address;
        std::uint16_t       opcode;
        std::uint16_t       size;
        std::uint8_t        flags;
        std::uint8_t        operand_count;
        operand             operand_list[24];
    };

    auto instruction_name(instruction const & in) noexcept -> std::string_view;
}
