# include <processor/output.hpp>
# include <processor/module.hpp>
# include <engine/command_set.hpp>
# include <core/logger.hpp>

namespace idascm
{
    auto output::instruction(outctx_t & ctx) -> bool
    {
        assert(m_isa);
        auto command = m_isa->get_command(ctx.insn.itype);
        assert(command);
        IDASCM_LOG_T("output: %s", command->name);

        // ctx.out_mnemonic();
        ctx.out_custom_mnem(command->name);

        auto const count = insn_operand_count(ctx.insn);
        for (std::uint8_t n = 0; n < count; ++ n)
        {
            if (n < UA_MAXOP)
            {
                if (! ctx.insn.ops[n].shown())
                    continue;

                if (ctx.insn.ops[n].type == o_void)
                {
                    break;
                }

                ctx.out_one_operand(n);
            }
            else
            {
                ctx.out_line("<OP>");
            }

            if (n + 1 < count && ctx.insn.ops[n + 1].type != o_void)
            {
                ctx.out_symbol(',');
                ctx.out_char(' ');
            }
        }

        // ctx.gen_cmt_line("gen_cmt_line");
        ctx.flush_outbuf();
        return true;
    }

    void output::mnemonics(outctx_t & ctx)
    {
    }

    auto output::operand(outctx_t & ctx, op_t const & op) -> bool
    {
        switch (op.type)
        {
            case o_void:
            {
                return false;
            }
            case o_imm:
            {
                switch (op.dtype)
                {
                    case dt_float:
                    {
                        char string[16] = { 0 };
                        qsnprintf(string, sizeof(string) - 1, "%g", *reinterpret_cast<float const*>(&op.value));
                        // if (print_fpval(string, sizeof(string) - 1, &op.value, 4))
                        if (true)
                        {
                            ctx.out_line(string, COLOR_NUMBER);
                        }
                        else
                        {
                            ctx.out_tagon(COLOR_ERROR);
                            ctx.out_btoa(op.value, 16);
                            ctx.out_tagoff(COLOR_ERROR);
                            remember_problem(PR_DISASM, ctx.insn.ea);
                        }
                        break;
                    }
                    case dt_string:
                    {
                        char string[16] = { 0 };
                        get_bytes(string, 8, op.addr);
                        // IDASCM_LOG_T("string: '%s'", string);
                        ctx.out_tagon(COLOR_DSTR);
                        ctx.out_char('\'');
                        ctx.out_line(string);
                        ctx.out_char('\'');
                        ctx.out_tagoff(COLOR_DSTR);
                        break;
                    }
                    default:
                    {
                        ctx.out_value(op, OOF_NUMBER | OOFS_IFSIGN | OOFW_IMM);
                        break;
                    }
                }
                return true;
            }
            case o_near:
            case o_mem:
            {
                if (! ctx.out_name_expr(op, op.addr))
                {
                    ctx.out_tagon(COLOR_ERROR);
                    ctx.out_btoa(op.addr, 16);
                    ctx.out_tagoff(COLOR_ERROR);
                    remember_problem(PR_NONAME, ctx.insn.ea);
                }
                return true;
            }
        }
        return false;
    }
}

void idaapi out_insn(outctx_t & ctx)
{
    assert(false);
}

void idaapi out_mnem(outctx_t & ctx)
{
    assert(false);
}

auto idaapi out_opnd(outctx_t & ctx, op_t const & op) -> bool
{
    assert(false);
    return false;
}
