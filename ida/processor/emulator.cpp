# include <ida/processor/emulator.hpp>
# include <ida/processor/analyzer.hpp>
# include <engine/command_set.hpp>
# include <engine/command.hpp>
# include <core/logger.hpp>

namespace idascm
{
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
            case o_displ:
            {
                auto const value = op_value(op);
                if (op.type == o_displ)
                {
                    switch (value.array.type)
                    {
                        case operand_type::int32:
                            create_dword(op.addr, value.array.size);
                            break;
                        case operand_type::float32:
                            create_float(op.addr, value.array.size);
                            break;
                        case operand_type::string8:
                            break;
                        case operand_type::string16:
                            break;
                    }
                }
                else
                {
                    insn.create_op_data(op.addr, op);
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
                    auto segment = getseg(insn.ea);
                    if (segment)
                    {
                        address = segment->start_ea + address;
                    }
                    else
                    {
                        IDASCM_LOG_W("no segment found");
                    }
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
                if (command->flags & command_flag_jump)
                {
                    if (op.type == o_far)
                    {
                        insn.add_cref(address, op.offb, fl_JF);
                        break;
                    }
                    insn.add_cref(address, op.offb, fl_JN);
                    break;
                }
                insn.add_dref(address, op.offb, dr_O);
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
        IDASCM_LOG_D("emulate +0x%04x %s flags=0x%02x", insn.ea, command->name.c_str(), command->flags);

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
        if (! command)
        {
            return false;
        }
        if (command->flags & command_flag_return)
            return true;
        return false;
    }
}
