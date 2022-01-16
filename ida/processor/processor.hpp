# pragma once
# include <ida/base/base.hpp>
# include <engine/version.hpp>
# include <engine/instruction.hpp>
# if IDA_SDK_VERSION >= 750
#   define IDASCM_DYNAMIC_MODULE_INSTANCE
# endif
# if 0
#   define IDASCM_FILL_INSTRUCTION_LIST
# endif

namespace idascm
{
    enum
    {
        processor_id = 0x8000 | 0xeaf,
    };

    class command_set;
    class command_manager;

    auto processor_command_manager(void) -> command_manager &;

    void processor_set_current_isa(command_set const * set);
    auto processor_current_isa(void) -> command_set const *;
    auto processor_isa(version ver) -> command_set const *;
    auto processor_version(int proc_id) noexcept -> version;

    auto to_string(processor_t::event_t event) -> char const *;

    constexpr
    void insn_set_operand_count(insn_t & insn, std::uint8_t count) noexcept
    {
        insn.insnpref = (insn.insnpref & ~0x1f) | (0x1f & count);
    }

    constexpr
    auto insn_operand_count(insn_t const & insn) noexcept -> std::uint8_t
    {
        return std::uint8_t(0x1f & insn.insnpref);
    }

    // 'not'
    constexpr
    void insn_set_inversion_flag(insn_t & insn, bool flag) noexcept
    {
        insn.insnpref = (insn.insnpref & ~0x20) | (flag ? 0x20 : 0);
    }

    // 'not'
    constexpr
    auto insn_inversion_flag(insn_t & insn) noexcept -> bool
    {
        return (insn.insnpref & 0x20) != 0;
    }

    constexpr
    auto op_set_type(op_t & op, operand_type type) noexcept
    {
        op.specflag1 = static_cast<char>(to_uint(type));
    }

    constexpr
    auto op_type(op_t const & op) noexcept
    {
        return to_operand_type(static_cast<std::uint8_t>(op.specflag1));
    }

    constexpr
    auto op_set_array_size(op_t & op, std::uint8_t size) noexcept
    {
        op.specflag2 = static_cast<char>(size);
    }

    constexpr
    auto op_array_size(op_t const & op) noexcept
    {
        return static_cast<std::uint8_t>(op.specflag2);
    }
}

idaman processor_t ida_module_data LPH;
