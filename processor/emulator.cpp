# include <processor/emulator.hpp>
# include <processor/analyzer.hpp>
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
            case o_mem:
            {
                insn.create_op_data(op.addr, op);
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
                if (command->flags & command_flag_jump)
                {
                    insn.add_cref(address, op.offb, fl_JN);
                    break;
                }
                if (command->flags & command_flag_call)
                {
                    insn.add_cref(address, op.offb, fl_CN);
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

    auto emulator::emulate_instruction(insn_t const & insn) -> bool
    {
        assert(m_isa);
        auto const command = m_isa->get_command(insn.itype);
        assert(command);
        if (! command)
        {
            return false;
        }
        IDASCM_LOG_D("emulate +0x%04x %s flags=0x%02x", insn.ea, command->name, command->flags);
        for (std::uint8_t i = 0; i < std::size(insn.ops); ++ i)
        {
            emulate_operand(insn, insn.ops[i]);
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

    auto emulator::get_autocomment(insn_t const & insn) const -> qstring
    {
        assert(m_isa);
        auto const command = m_isa->get_command(insn.itype);
        assert(command);

        qstring comment;
        comment.reserve(128);
        if (command)
        {
            char buffer[32];
            qsnprintf(buffer, sizeof(buffer) - 1, "0x%04x", insn.itype);
            comment.append(buffer);

            if (command->comment[0])
            {
                comment.append(" - ");
                comment.append(command->comment);
            }

            qstring flags;
            flags.reserve(128);
            for (std::uint8_t i = 0; i < 8; ++ i)
            {
                std::uint8_t const flag = 1 << i;
                if (command->flags & flag)
                {
                    if (! flags.empty())
                        flags.append(", ");
                    flags.append(to_string(command_flag(flag)));
                }
            }
            if (! flags.empty())
            {
                comment.append(" (flags: ");
                comment.append(flags);
                comment.append(")");
            }
        }

        return comment;
    }
}
