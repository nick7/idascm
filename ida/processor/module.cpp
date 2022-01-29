# include <ida/processor/module.hpp>
# include <ida/processor/analyzer.hpp>
# include <ida/processor/emulator.hpp>
# include <ida/processor/output.hpp>
# include <engine/command.hpp>
# include <engine/command_manager.hpp>
# include <engine/command_set.hpp>
# include <core/json.hpp>
# include <core/logger.hpp>

namespace idascm
{
    module::module(void)
        : m_data_id(-1)
        , m_isa(nullptr)
        , m_analyzer(nullptr)
        , m_emulator(nullptr)
        , m_output(nullptr)
    {}

    auto module::set_version(version ver) -> bool
    {
        IDASCM_LOG_I("Initializing '%s'", to_string(ver));
        m_isa = processor_isa(ver);
        delete m_analyzer;
        delete m_emulator;
        delete m_output;
        m_analyzer = nullptr;
        m_emulator = nullptr;
        m_output   = nullptr;
        if (! m_isa)
        {
            IDASCM_LOG_W("Unable to retrieve ISA for '%s'", to_string(ver));
            return false;
        }

        processor_set_current_isa(m_isa);

        m_analyzer  = new analyzer(version_game(ver));
        m_emulator  = new emulator;
        m_output    = new output;

        m_analyzer->set_isa(m_isa);
        m_emulator->set_isa(m_isa);
        m_emulator->set_analyzer(m_analyzer);
        m_output->set_isa(m_isa);
        m_output->set_analyzer(m_analyzer);

        return true;
    }

    module::~module(void)
    {
        if (m_analyzer)
        {
            delete m_analyzer;
            m_analyzer = nullptr;
        }
        if (m_emulator)
        {
            delete m_emulator;
            m_emulator = nullptr;
        }
        if (m_output)
        {
            delete m_output;
            m_output = nullptr;
        }
        m_isa = nullptr;
    }

    namespace
    {
        auto get_command(command_set const * isa, insn_t const * ins) -> command const *
        {
            if (! isa || ! ins)
                return nullptr;
            return isa->get_command(ins->itype);
        }
    }

    // virtual
    ssize_t idaapi module::on_event(ssize_t msgid, va_list args)
    {
        IDASCM_LOG_T("module::on_event: %zd '%s'", msgid, to_string(processor_t::event_t(msgid)));
        switch (msgid)
        {
            case processor_t::ev_init: // 0
            {
# if IDA_SDK_VERSION >= 730
                inf_set_be(false);
                inf_set_gen_lzero(true);
# endif
                return 1;
            }
            case processor_t::ev_term: // 1
            {
# if IDA_SDK_VERSION >= 750
                clr_module_data(m_data_id);
# endif
                return 1;
            }
            case processor_t::ev_newprc: // 2
            {
                auto const id = va_arg(args, int);
                auto const keep_cfg = va_arg(args, bool);
                IDASCM_LOG_D("ev_newprc: %d, %d", id, keep_cfg);
                auto const ver = processor_version(id);
                if (ver == version::unknown)
                    return -1;
                set_version(ver);
                return 1;
            }
            case processor_t::ev_newfile: // 4
            {
                IDASCM_LOG_D("ev_newfile: '%s'", va_arg(args, char const *));
                return 1;
            }
            case processor_t::ev_ana_insn:
            {
                auto insn = va_arg(args, insn_t *);
                assert(insn);
                assert(m_analyzer);
                return m_analyzer->analyze_instruction(*insn) ? insn->size : 0;
            }
            case processor_t::ev_emu_insn:
            {
                auto const insn = va_arg(args, insn_t const *);
                assert(insn);
                return m_emulator->emulate_instruction(*insn) ? 1 : 0;
            }
            case processor_t::ev_out_header:
            {
                auto const ctx = va_arg(args, outctx_t *);
                ctx->gen_header(GH_PRINT_ALL);
                return 1;
            }
            case processor_t::ev_out_insn:
            {
                auto const ctx = va_arg(args, outctx_t *);
                return m_output->output_instruction(*ctx) ? 1 : -1;
            }
            case processor_t::ev_out_operand:
            {
                auto const ctx = va_arg(args, outctx_t *);
                auto const op  = va_arg(args, op_t const *);
                // return out_opnd(*ctx, *op) ? 1 : -1;
                return m_output->output_operand(*ctx, *op) ? 1 : -1;
            }
            case processor_t::ev_is_cond_insn:
            {
                auto const cmd = get_command(m_isa, va_arg(args, insn_t const *));
                assert(cmd);
                if (cmd->flags & command_flag_condition)
                    return 1;
                return -1;
            }
            case processor_t::ev_is_call_insn:
            {
                auto const cmd = get_command(m_isa, va_arg(args, insn_t const *));
                assert(cmd);
                if (cmd->flags & command_flag_call)
                    return 1;
                return -1;
            }
            case processor_t::ev_is_ret_insn:
            {
                auto const insn = va_arg(args, insn_t const *);
                return m_emulator->is_return(*insn) ? 1 : -1;
            }
            case processor_t::ev_get_reg_name:
            {
                auto const buf      = va_arg(args, qstring *);
                auto const reg      = va_arg(args, int);
                auto const width    = va_arg(args, std::size_t);
                auto const reghi    = va_arg(args, int);
                assert(buf);
                buf->sprnt("@%d", reg);
                return buf->length();
            }
            case processor_t::ev_str2reg:
            {
                auto const regname = va_arg(args, char const *);
                assert(regname && regname[0]);
                if (regname[0] == '@')
                {
                    return 1 + std::atoi(regname + 1);
                }
                return 0;
            }
            case processor_t::ev_get_autocmt:
            {
                auto const buf  = va_arg(args, qstring *);
                auto const insn = va_arg(args, insn_t const *);
                assert(buf && insn);
                auto const comment = m_output->get_autocomment(*insn);
                if (comment.empty())
                    return 0;
                *buf = comment;
                return 1;
            }
        }

        return 0;
    }
}
