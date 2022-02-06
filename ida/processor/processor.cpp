# include <ida/processor/processor.hpp>
# include <ida/processor/module.hpp>
# include <engine/command_set.hpp>
# include <engine/command_manager.hpp>
# include <core/logger.hpp>
# include <windows.h>

namespace idascm
{
    command_set const * g_isa;
    int                 g_proc;

# if ! defined IDASCM_DYNAMIC_MODULE_INSTANCE
    auto module_instance(void) -> module &
    {
        static module instance;
        return instance;
    }
# endif

    namespace
    {
        void startup(void)
        {
            initialize_ida_logger();
            IDASCM_LOG_I("idascm %s", build_version());
            IDASCM_LOG_I("IDA_SDK_VERSION: %d", IDA_SDK_VERSION);
        }

        char const * const g_register_names[] = \
        {
            "@CS", "@DS"
        };

        instruc_t g_instruction_list[0x1000];

        auto idaapi notify_handler(void * user_data, int code, va_list va) -> ssize_t
        {
            static bool is_initialized = false;
            if (! is_initialized)
            {
                startup();
            }
            is_initialized = true;
# if ! defined IDASCM_DYNAMIC_MODULE_INSTANCE
            return module_instance().on_event(code, va);
# else
            IDASCM_LOG_D("notify_handler: %d '%s'", code, to_string(processor_t::event_t(code)));
            switch (code)
            {
                case processor_t::ev_get_procmod:
                {
                    int data_id = 0;
                    auto m = static_cast<module *>(set_module_data(&data_id, new module));
                    m->set_data_id(data_id);
                    return reinterpret_cast<std::size_t>(m);
                }
            }
            return 0;
# endif
        }

        auto data_root_path(void) -> char const *
        {
            static char root[1024];

            CHAR path[1024] = {};
            GetModuleFileNameA(GetModuleHandleA(NULL), path, sizeof(path) - 1);

            CHAR * file = nullptr;
            GetFullPathNameA(path, sizeof(root), root, &file);
            if (file)
            {
                *file = 0;
            }
            qstrncat(root, "cfg\\idascm\\", sizeof(root) - 1);
            return root;
        }
    }

    auto processor_command_manager(void) -> command_manager &
    {
        static command_manager manager(data_root_path());
        return manager;
    }

    void processor_set_current_isa(command_set const * isa)
    {
        static decltype(g_instruction_list[0].feature) const use_map[] = \
        {
            CF_USE1, CF_USE2, CF_USE3, CF_USE4, CF_USE5, CF_USE6,
# if IDA_SDK_VERSION >= 730
            CF_USE7, CF_USE8,
# endif
        };
        IDASCM_LOG_I("processor_set_isa: %s", isa ? version_to_string(isa->get_version()).data() : nullptr);
        g_isa = isa;

        if (g_isa && false)
        {
            for (std::uint16_t op = 0; op < std::size(g_instruction_list); ++ op)
            {
                g_instruction_list[op] = {};
                if (auto const cmd = g_isa->get_command(op))
                {
                    g_instruction_list[op].name    = cmd->name.c_str();
                    g_instruction_list[op].feature = 0;
                    for (std::size_t i = 0; i < cmd->arguments.size(); ++ i)
                    {
                        if (i < std::size(use_map))
                            g_instruction_list[op].feature |= use_map[i];
                    }
                    if (cmd->flags & command_flag_branch)
                        g_instruction_list[op].feature |= CF_JUMP;
                    if (cmd->flags & command_flag_call)
                        g_instruction_list[op].feature |= CF_CALL;
                    if (cmd->flags & command_flag_stop)
                        g_instruction_list[op].feature |= CF_STOP;
                }
            }
        }
    }

    auto processor_current_isa(void) -> command_set const *
    {
        return g_isa;
    }

    auto processor_isa(version ver) -> command_set const *
    {
        return processor_command_manager().get_set(ver);
    }

    namespace
    {
        version const gs_processor_table[] = \
        {
            version::gta3_ps2,
            version::gta3_ps2_ex,
            version::gta3_xbox,
            version::gta3_pc,
            version::gta3_pc_ex,

            version::gtavc_ps2,
            version::gtavc_xbox,
            version::gtavc_pc,

            version::gtasa_ps2,
            version::gtasa_xbox,
            version::gtasa_pc,

            version::gtalcs_psp,
            version::gtalcs_ps2,

            version::gtavcs_psp,
            version::gtavcs_ps2,

            version::gta3_pc_cleo,
            version::gtavc_pc_cleo,
            version::gtasa_pc_cleo,

            version::gta3_custom,
            version::gtavc_custom,
            version::gtasa_custom,
            version::gtalcs_custom,
            version::gtavcs_custom,

            version::gta3_anniversary,
            version::gtavc_anniversary,
            version::gtasa_anniversary,
            version::gtalcs_anniversary,

            version::gta3_definitive,
            version::gtavc_definitive,
            version::gtasa_definitive,
        };
    }

    auto processor_version(int proc_id) noexcept -> version
    {
        if (proc_id >= 0 && proc_id < std::size(gs_processor_table))
            return gs_processor_table[proc_id];
        return version::unknown;
    }

    auto processor(void) noexcept -> processor_t
    {
        static char const * s_snames[std::size(gs_processor_table) + 1];
        static char const * s_lnames[std::size(gs_processor_table) + 1];
        for (std::size_t i = 0; i < std::size(gs_processor_table); ++ i)
        {
            s_snames[i] = version_to_string(gs_processor_table[i]).data();
            s_lnames[i] = version_description(gs_processor_table[i]).data();
        }

        static char l_assembler_header_string[256];
        qsnprintf(l_assembler_header_string, sizeof(l_assembler_header_string) - 1, "// idascm %s", build_version());

        static char const * const l_assembler_header[] = \
        {
            l_assembler_header_string,
            nullptr,
        };

        asm_t assembler = {};
        assembler.flag              = AS_COLON | ASH_HEXF3 | ASO_OCTF1 | ASB_BINF3;
        assembler.uflag             = 0;
        assembler.name              = "GTA SCM";
        assembler.help              = 0;
        assembler.header            = l_assembler_header;
        assembler.origin            = "org";
        assembler.end               = "end";
        assembler.cmnt              = "//";
        assembler.ascsep            = '"';
        assembler.accsep            = '\'';
        assembler.esccodes          = "\"'";
        assembler.a_ascii           = ".ascii";
        assembler.a_byte            = ".byte";
        assembler.a_word            = ".word";
        assembler.a_dword           = ".dword";
        assembler.a_qword           = ".qword";     // 8 bytes
        assembler.a_oword           = ".oword";     // 16 bytes
        assembler.a_float           = ".float";
        assembler.a_double          = nullptr;
        assembler.a_tbyte           = nullptr;
        assembler.a_packreal        = nullptr;
        assembler.a_dups            = ".dup #s(c,) #d, #v";
        assembler.a_bss             = ".bss %s";
        assembler.a_equ             = "equ";
        assembler.a_seg             = nullptr;
        assembler.a_curip           = "*";
        // assembler.out_func_header   = nullptr;
        // assembler.out_func_footer   = nullptr;
        assembler.a_public          = "global";
        assembler.a_weak            = nullptr;
        assembler.a_extrn           = "xref";
        assembler.a_comdef          = nullptr;
        assembler.get_type_name     = nullptr;
        assembler.a_align           = nullptr;
        assembler.lbrace            = '(';
        assembler.rbrace            = ')';
        assembler.a_mod             = "%";
        assembler.a_band            = "&";
        assembler.a_bor             = "|";
        assembler.a_xor             = "^";
        assembler.a_bnot            = "!";
        assembler.a_shl             = "<<";
        assembler.a_shr             = ">>";
        assembler.a_sizeof_fmt      = "sizeof";
        assembler.flag2             = AS2_BYTE1CHAR;
        assembler.cmnt2             = nullptr;
        assembler.low8              = nullptr;
        assembler.high8             = nullptr;
        assembler.low16             = nullptr;
        assembler.high16            = nullptr;
        assembler.a_include_fmt     = nullptr;
        assembler.a_vstruc_fmt      = nullptr;
        assembler.a_rva             = nullptr;
        // assembler.a_yword           = nullptr;

        static asm_t const s_asm = assembler;
        static asm_t const * const s_asm_list[] = \
        {
            &s_asm,
            nullptr,
        };

        processor_t proc = {};
        proc.version        = IDP_INTERFACE_VERSION;
        proc.id             = processor_id();
        proc.flag           = PR_USE32 | PR_DEFSEG32 | PRN_HEX | PR_BINMEM | PR_NO_SEGMOVE | PR_CNDINSNS;
        // proc.flag2          = 0;
        proc.cnbits         = 8;
        proc.dnbits         = 8;
        proc.psnames        = s_snames;
        proc.plnames        = s_lnames;
        proc.assemblers     = s_asm_list;
        proc._notify        = notify_handler;
        proc.reg_names      = g_register_names;
        proc.regs_num       = qnumber(g_register_names);
        proc.reg_first_sreg = 0;
        proc.reg_last_sreg  = 1;
        proc.segreg_size    = 0;
        proc.reg_code_sreg  = 0;
        proc.reg_data_sreg  = 1;
        // proc.codestart      = g_codestart_list;
        // proc.retcodes       = g_retcode_list;
        proc.instruc_start  = 0x0000;
        proc.instruc_end    = 0x0fff;
        proc.instruc        = g_instruction_list;
        return proc;
    }

# if IDA_SDK_VERSION >= 700
    auto to_string(processor_t::event_t event) -> char const *
    {
        switch (event)
        {
            case processor_t::ev_init: // 0
                return "init";
            case processor_t::ev_term: // 1
                return "term";
            case processor_t::ev_newprc: // 2
                return "newprc";
            case processor_t::ev_newasm: // 3
                return "newasm";
            case processor_t::ev_newfile: // 4
                return "newfile";
            case processor_t::ev_oldfile: // 5
                return "oldfile";
            case processor_t::ev_newbinary: // 6
                return "newbinary";
            case processor_t::ev_endbinary: // 7
                return "endbinary";
            case processor_t::ev_ana_insn: // 10
                return "ana_insn";
            case processor_t::ev_emu_insn: // 11
                return "emu_insn";
            case processor_t::ev_out_header: // 12
                return "out_header";
            case processor_t::ev_out_segstart: // 14
                return "out_segstart";
            case processor_t::ev_out_assumes: // 16
                return "out_assumes";
            case processor_t::ev_out_insn: // 17
                return "out_insn";
            case processor_t::ev_out_mnem: // 18
                return "out_mnem";
            case processor_t::ev_out_operand: // 19
                return "out_operand";
            case processor_t::ev_out_data: // 20
                return "out_data";
            case processor_t::ev_out_label: // 21
                return "out_label";
            case processor_t::ev_creating_segm: // 26
                return "creating_segm";
            case processor_t::ev_undefine: // 29
                return "undefine";
            case processor_t::ev_treat_hindering_item: // 30
                return "treat_hindering_item";
            case processor_t::ev_rename: // 31
                return "rename";
            case processor_t::ev_is_sane_insn: // 33
                return "is_sane_insn";
            case processor_t::ev_is_ret_insn: // 36
                return "is_ret_insn";
            case processor_t::ev_is_insn_table_jump: // 40
                return "is_insn_table_jump";
            case processor_t::ev_is_switch: // 41
                return "is_switch";
            case processor_t::ev_is_align_insn: // 44
                return "is_align_insn";
            case processor_t::ev_can_have_type: // 48
                return "can_have_type";
            case processor_t::ev_get_reg_name: // 52
                return "get_reg_name";
            case processor_t::ev_str2reg: // 53
                return "str2reg";
            case processor_t::ev_get_autocmt: // 54
                return "get_autocmt";
            case processor_t::ev_get_bg_color: // 55
                return "get_bg_color";
            case processor_t::ev_is_jump_func: // 56
                return "is_jump_func";
            case processor_t::ev_func_bounds: // 57
                return "func_bounds";
            case processor_t::ev_create_func_frame: // 60
                return "create_func_frame";
            case processor_t::ev_demangle_name: // 63
                return "demangle_name";
            case processor_t::ev_add_cref: // 64
                return "add_cref";
            case processor_t::ev_add_dref: // 65
                return "add_dref";
            case processor_t::ev_del_cref: // 66
                return "del_cref";
            case processor_t::ev_del_dref: // 67
                return "del_dref";
            case processor_t::ev_coagulate_dref: // 68
                return "coagulate_dref";
            case processor_t::ev_auto_queue_empty: // 71
                return "auto_queue_empty";
            case processor_t::ev_extract_address: // 75
                return "extract_address";
            case processor_t::ev_realcvt: // 76
                return "realcvt";
            case processor_t::ev_analyze_prolog: // 81
                return "analyze_prolog";
# if IDA_SDK_VERSION >= 750
            case processor_t::ev_get_procmod: // 90
                return "get_procmod";
            case processor_t::ev_asm_installed: // 91
                return "asm_installed";
# endif
            case processor_t::ev_get_reg_info: // 1008
                return "get_reg_info";
        }
        return nullptr;
    }
# endif

    void op_set_value(op_t & op, operand_value const & value) noexcept
    {
        op_set_value_uint64(op, value.uint64);
    }

    auto op_value(op_t const & op) noexcept -> operand_value
    {
        operand_value value;
        value.uint64 = op_value_uint64(op);
        return value;
    }
}

processor_t LPH = idascm::processor();
