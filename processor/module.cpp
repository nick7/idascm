# include <processor/module.hpp>
# include <processor/analyzer.hpp>
# include <processor/emulator.hpp>
# include <processor/output.hpp>
# include <engine/command.hpp>
# include <engine/command_manager.hpp>
# include <engine/command_set.hpp>
# include <engine/decoder.hpp>
# include <core/json.hpp>
# include <core/logger.hpp>

namespace idascm
{
    module::module(void)
        : m_proc(-1)
        , m_data_id(-1)
        , m_isa(nullptr)
        , m_analyzer(new analyzer)
        , m_emulator(new emulator)
        , m_output(new output)
    {
        m_isa = processor_isa(version::gtavc_win32);
        assert(m_isa);
        processor_set_current_isa(m_isa);
        m_analyzer->set_isa(m_isa);
        m_emulator->set_isa(m_isa);
        m_output->set_isa(m_isa);
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

    auto get_command(command_set const * isa, insn_t const * ins) -> command const *
    {
        if (! isa || ! ins)
            return nullptr;
        return isa->get_command(ins->itype);
    }

    // virtual
    ssize_t idaapi module::on_event(ssize_t msgid, va_list va)
    {
        IDASCM_LOG_D("module::on_event - %d '%s'", msgid, to_string(processor_t::event_t(msgid)));
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
                m_proc = va_arg(va, int);
                auto const keep_cfg = va_arg(va, bool);
                return 1;
            }
            case processor_t::ev_newfile: // 4
            {
                // here we can load additional data from a current dir
                auto * fname = va_arg(va, char*);
                return 1;
            }
            case processor_t::ev_is_cond_insn:
            {
                auto const cmd = get_command(m_isa, va_arg(va, insn_t const *));
                assert(cmd);
                if (cmd->flags & command_flag_condition)
                    return 1;
                return -1;
            }
            case processor_t::ev_is_ret_insn:
            {
                auto const cmd = get_command(m_isa, va_arg(va, insn_t const *));
                assert(cmd);
                if (cmd->flags & command_flag_return)
                    return 1;
                return -1;
            }
            case processor_t::ev_is_call_insn:
            {
                auto const cmd = get_command(m_isa, va_arg(va, insn_t const *));
                assert(cmd);
                if (cmd->flags & command_flag_call)
                    return 1;
                return -1;
            }
            case processor_t::ev_ana_insn:
            {
                auto out = va_arg(va, insn_t *);
                return m_analyzer->analyze(out);
            }
            case processor_t::ev_emu_insn:
            {
                auto const insn = va_arg(va, insn_t const *);
                assert(insn);
                return m_emulator->emulate(*insn);
            }
            case processor_t::ev_out_header:
            {
                auto const ctx = va_arg(va, outctx_t *);
                ctx->gen_header(GH_PRINT_ALL);
                return 1;
            }
            case processor_t::ev_out_insn:
            {
                auto const ctx = va_arg(va, outctx_t *);
                return m_output->instruction(*ctx) ? 1 : -1;
            }
            case processor_t::ev_out_operand:
            {
                auto const ctx = va_arg(va, outctx_t *);
                auto const op  = va_arg(va, op_t const *);
                // return out_opnd(*ctx, *op) ? 1 : -1;
                return m_output->operand(*ctx, *op) ? 1 : -1;
            }
            // case processor_t::ev_realcvt: // 76
            // {
            //     auto m    = va_arg(va, void *);
            //     auto e    = va_arg(va, uint16 *);
            //     auto swt  = va_arg(va, uint16);
            //     return 0;
            // }
        }

        return 0;
    }
}
