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
                op_num(insn.ea, op.n);
                break;
            }
            case o_mem:
            {
                insn.create_op_data(op.addr, op);
                insn.add_dref(op.addr, op.offb, dr_R);
                break;
            }
            case o_near:
            {
                if (command->flags & command_flag_jump)
                {
                    insn.add_cref(op.addr, op.offb, fl_JN);
                    break;
                }
                if (command->flags & command_flag_call)
                {
                    insn.add_cref(op.addr, op.offb, fl_CN);
                    break;
                }
                insn.add_dref(op.addr, op.offb, dr_O);
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

    auto emulator::emulate(insn_t const & insn) -> int
    {
        assert(m_isa);
        auto const command = m_isa->get_command(insn.itype);
        assert(command);
        if (! command)
        {
            return 0;
        }
        IDASCM_LOG_T("emulate %s", command->name);
        for (std::uint8_t i = 0; i < std::size(insn.ops); ++ i)
        {
            emulate_operand(insn, insn.ops[i]);
        }
        if (! (command->flags & command_flag_stop))
            insn.add_cref(insn.ea, insn.ea + insn.size, fl_F);
        return 1;
    }
}
