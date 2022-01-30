# pragma once
# include <engine/version.hpp>
# include <engine/instruction.hpp>
# if 1 // sdk shit fix
#   define print_predefined_segname static print_predefined_segname
#   include <idaidp.hpp>
#   undef print_predefined_segname
# else
#   include <idaidp.hpp>
# endif
# if IDA_SDK_VERSION < 750
#   define IDASCM_STATIC_MODULE_INSTANCE
# endif

namespace idascm
{
    class command_set;
    class command_manager;

    constexpr
    auto processor_id(void) noexcept -> std::uint16_t
    {
        return 0x8000 | ('s' << 0) | ('c' << 8);
    }

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
        op.specflag1 = static_cast<char>(type);
    }

    constexpr
    auto op_type(op_t const & op) noexcept
    {
        return static_cast<operand_type>(op.specflag1);
    }

    // constexpr
    // auto op_set_array_size(op_t & op, std::uint8_t size) noexcept
    // {
    //     op.specflag2 = static_cast<char>(size);
    // }

    // constexpr
    // auto op_array_size(op_t const & op) noexcept
    // {
    //     return static_cast<std::uint8_t>(op.specflag2);
    // }

    constexpr
    void op_set_value_uint64(op_t & op, std::uint64_t value) noexcept
    {
        op.value    = static_cast<std::uint32_t>(value >>  0);
        op.specval  = static_cast<std::uint32_t>(value >> 32);
    }

    constexpr
    auto op_value_uint64(op_t const & op) noexcept -> std::uint64_t
    {
        return static_cast<std::uint64_t>(op.value) | (static_cast<std::uint64_t>(op.specval) << 32);
    }

    void op_set_value(op_t & op, operand_value const & value) noexcept;
    auto op_value(op_t const & op) noexcept -> operand_value;
}

idaman processor_t ida_module_data LPH;
