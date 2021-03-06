# include <ida/processor/output.hpp>
# include <ida/processor/analyzer.hpp>
# include <ida/processor/module.hpp>
# include <engine/command_set.hpp>
# include <engine/instruction.hpp>
# include <core/logger.hpp>

namespace idascm
{
    auto output::output_instruction(outctx_t & ctx) -> bool
    {
        assert(m_isa);
        auto command = m_isa->get_command(ctx.insn.itype);
        assert(command);
        if (! command)
        {
            return false;
        }
        IDASCM_LOG_T("output: %s", command->name);

        output_mnemonics(ctx);

        auto const count = insn_operand_count(ctx.insn);

        instruction src = {}; // actual instruction
        if (m_analyzer && count > UA_MAXOP)
        {
            m_analyzer->analyze_instruction(ctx.insn.ea, src);
        }

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
                if (m_analyzer)
                {
                    op_t op = {};
                    op.n = n;
                    if (m_analyzer->handle_operand(src, n, op))
                    {
                        output_operand(ctx, op);
                    }
                }
                else
                {
                    ctx.out_line("<OP>");
                }
            }

            if (n + 1 < count)
            {
                ctx.out_symbol(',');
                ctx.out_char(' ');
            }
        }

        // ctx.gen_cmt_line("gen_cmt_line");
        ctx.flush_outbuf();
        return true;
    }

    void output::output_mnemonics(outctx_t & ctx)
    {
        auto command = m_isa->get_command(ctx.insn.itype);
        assert(command);
        // ctx.out_mnemonic();
        if (insn_inversion_flag(ctx.insn))
        {
            ctx.out_keyword("NOT");
            ctx.out_char(' ');
        }
        if (command->name[0])
        {
            ctx.out_custom_mnem(command->name);
        }
        else
        {
            char name[32];
            qsnprintf(name, sizeof(name) - 1, "UNKNOWN_0x%04x", ctx.insn.itype);
            ctx.out_custom_mnem(name);
        }
    }

    namespace
    {
        auto float_suffix(operand_type type) noexcept -> char const *
        {
            switch (type)
            {
                case operand_type::float0:
                    return "f0";
                case operand_type::float8:
                    return "f8";
                case operand_type::float16:
                    return "f16";
                case operand_type::float24:
                    return "f24";
                case operand_type::float32:
                    return "f";
            }
            return nullptr;
        }
    }

    auto output::output_operand(outctx_t & ctx, op_t const & op) -> bool
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
                    case dt_packreal:
                    {
                        char string[32] = { 0 };
                        qsnprintf(string, sizeof(string) - 1, "%g", static_cast<int>(op.value) / 16.f);
                        ctx.out_line(string, COLOR_NUMBER);
                        break;
                    }
                    case dt_float:
                    {
                        char string[32] = { 0 };
                        qsnprintf(string, sizeof(string) - 1, "%g", *reinterpret_cast<float const *>(&op.value));
                        if (! strchr(string, '.'))
                        {
                            qstrncat(string, ".", sizeof(string) - 1);
                        }
                        qstrncat(string, float_suffix(op_type(op)), sizeof(string) - 1);
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
            // @REG
            case o_reg:
            {
                char string[32];
                qsnprintf(string, sizeof(string) - 1, "@%d", op.reg);
                ctx.out_register(string);
                return true;
            }
            // @REG[@REG, SIZE]
            case o_phrase:
            {
                char string[32];
                qsnprintf(string, sizeof(string) - 1, "@%d", op.addr);
                ctx.out_register(string);
                ctx.out_symbol('[');
                qsnprintf(string, sizeof(string) - 1, "@%d", op.reg);
                ctx.out_register(string);
                ctx.out_symbol(',');
                qsnprintf(string, sizeof(string) - 1, "%d", op_array_size(op));
                ctx.out_line(string, COLOR_NUMBER);
                ctx.out_symbol(']');
                return true;
            }
            case o_displ:   // GLOBAL[@REG, SIZE]
            case o_mem:     // GLOBAL
            case o_far:
            case o_near:
            {
                std::uint32_t address = op.addr;
                if (o_near == op.type)
                {
                    auto segment = getseg(ctx.insn.ea);
                    if (segment)
                    {
                        address = segment->start_ea + address;
                    }
                }
                if (! ctx.out_name_expr(op, address))
                {
                    ctx.out_tagon(COLOR_ERROR);
                    ctx.out_btoa(op.value, 16);
                    ctx.out_tagoff(COLOR_ERROR);
                    remember_problem(PR_NONAME, ctx.insn.ea);
                }
                if (o_displ == op.type)
                {
                    ctx.out_symbol('[');
                    char string[32];
                    qsnprintf(string, sizeof(string) - 1, "@%d", op.reg);
                    ctx.out_register(string);
                    ctx.out_symbol(',');
                    ctx.out_printf("%d", op_array_size(op));
                    ctx.out_symbol(']');
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
