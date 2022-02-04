# include <ida/processor/emulator.hpp>
# include <ida/processor/analyzer.hpp>
# include <engine/command_set.hpp>
# include <engine/command.hpp>
# include <core/logger.hpp>

namespace idascm
{
    namespace
    {
        auto create_data(ea_t address, operand_type type, std::uint8_t count) -> bool
        {
            switch (type)
            {
                case operand_type::int32:
                    return create_dword(address, count * 4);
                case operand_type::float32:
                    return create_float(address, count * 4);
                case operand_type::string8:
                    return create_qword(address, count * 8);
                case operand_type::string16:
                    return create_oword(address, count * 16);
                default:
                    // actually dword is the smallest possible type
                    IDASCM_LOG_D("unspecified data type: %d", type);
                    return create_dword(address, count * 4);
            }
            return false;
        }
    }

    auto emulator::near_to_ea(ea_t ea, std::uint32_t address) const noexcept -> ea_t
    {
        auto segment = getseg(ea);
        if (! segment)
        {
            IDASCM_LOG_W("no segment found (ea: 0x%08zx)", static_cast<std::size_t>(ea));
            return BADADDR;
        }
        return segment->start_ea + address;
    }

    auto emulator::operand_address(ea_t ea, op_t const & op) const -> ea_t
    {
        std::uint32_t address = op.addr;
        if (op.type == o_far)
        {
            return address;
        }
        if (op.type == o_near)
        {
            return near_to_ea(ea, address);
        }
        return BADADDR;
    }

    void emulator::emulate_operand(insn_t const & insn, op_t const & op)
    {
        assert(m_isa);
        auto const command = m_isa->get_command(insn.itype);
        assert(command);
        switch (op.type)
        {
            case o_void:
            {
                break;
            }
            case o_imm:
            {
                set_immd(insn.ea);
                switch (op.dtype)
                {
                    case dt_byte:
                    case dt_word:
                    case dt_dword:
                        op_dec(insn.ea, op.n);
                        break;
                    case dt_float:
                    case dt_packreal:
                        op_flt(insn.ea, op.n);
                        break;
                    default:
                        op_num(insn.ea, op.n);
                        break;
                }
                break;
            }
            case o_reg:
            case o_phrase:
            {
                break;
            }
            case o_mem:
            {
                auto const value = op_value(op);
                switch (op_type(op))
                {
                    case operand_type::global:
                        if (! create_data(op.addr, value.variable.type, 1))
                        {
                            IDASCM_LOG_W("create_data failed (ea=0x%08x, type=%d)", op.addr, value.variable.type);
                        }
                        break;
                    default:
                        if (op.n < command->arguments.size())
                        {
                            switch (command->arguments[op.n].type)
                            {
                                case type::mission:
                                    break;
                            }
                        }
                        break;
                }
                // TODO: deal with dr_R / dw_W
                insn.add_dref(op.addr, op.offb, dr_O);
                break;
            }
            case o_displ:
            {
                auto const value = op_value(op);
                switch (op_type(op))
                {
                    case operand_type::global_array:
                        if (! create_data(op.addr, value.array.type, value.array.size))
                        {
                            IDASCM_LOG_W("create_data failed (ea=0x%08x, type=%d, count=%d)", op.addr, value.array.type, value.array.size);
                        }
                        break;
                    default:
                        IDASCM_LOG_W("unsupported type: %d", op_type(op), operand_type_to_string(op_type(op)));
                        break;
                }
                // TODO: deal with dr_R / dw_W
                insn.add_dref(op.addr, op.offb, dr_O);
                break;
            }
            case o_near:
            case o_far:
            {
                std::uint32_t address = op.addr;
                if (op.type == o_near)
                {
                    address = near_to_ea(insn.ea, address);
                }
                if (command->flags & command_flag_call)
                {
                    if (op.type == o_far)
                    {
                        insn.add_cref(address, op.offb, fl_CF);
                        break;
                    }
                    insn.add_cref(address, op.offb, fl_CN);
                    break;
                }
                if (command->flags & command_flag_branch)
                {
                    if (op.type == o_far)
                    {
                        insn.add_cref(address, op.offb, fl_JF);
                        break;
                    }
                    insn.add_cref(address, op.offb, fl_JN);
                    break;
                }
                IDASCM_LOG_W("0x%08x %s: unable to add cref", insn.ea, command->name.c_str());
                break;
            }
            default:
            {
                IDASCM_LOG_W("usupported operand type %d", op.type);
                break;
            }
        }
    }

    void emulator::set_isa(command_set const * isa)
    {
        m_isa = isa;
    }

    void emulator::set_analyzer(analyzer * analyzer)
    {
        m_analyzer = analyzer;
    }

    auto emulator::emulate_instruction(insn_t const & insn) -> bool
    {
        assert(m_isa);
        auto const command = m_isa->get_command(insn.itype);
        assert(command);
        if (! command)
        {
            return false;
        }
        IDASCM_LOG_T("emulate +0x%04x %s flags=0x%02x", insn.ea, command->name.c_str(), command->flags);

        auto const count = insn_operand_count(insn);
        instruction src = {};
        if (m_analyzer && count > std::size(insn.ops))
        {
            m_analyzer->analyze_instruction(insn.ea, src);
        }

        for (std::uint8_t n = 0; n < count; ++ n)
        {
            if (n < std::size(insn.ops))
            {
                emulate_operand(insn, insn.ops[n]);
            }
            else
            {
                op_t op = {};
                op.n = n;
                if (m_analyzer->handle_operand(src, n, op))
                {
                    emulate_operand(insn, op);
                }
            }
        }

        if (! (command->flags & command_flag_stop))
        {
            add_cref(insn.ea, insn.ea + insn.size, fl_F);
        }
        return true;
    }

    auto emulator::is_return(insn_t const & insn) const -> bool
    {
        assert(m_isa);
        auto const command = m_isa->get_command(insn.itype);
        assert(command);
        return command && (command->flags & command_flag_return);
    }

    auto emulator::is_switch(insn_t const & insn) const -> bool
    {
        assert(m_isa);
        auto const command = m_isa->get_command(insn.itype);
        assert(command);
        if (command && (command->flags & command_flag_switch))
        {
            if (! (command->flags & command_flag_dependent))
                return true;
        }
        return true;
    }
}
