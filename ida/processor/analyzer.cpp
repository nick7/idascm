# include <ida/processor/analyzer.hpp>
# include <ida/base/memory_ida.hpp>
# include <engine/command.hpp>
# include <engine/command_set.hpp>
# include <engine/decoder.hpp>
# include <engine/instruction.hpp>
# include <engine/loader.hpp>
# include <core/logger.hpp>
# include <cassert>

namespace idascm
{
    analyzer::analyzer(game game, command_set const & isa, memory_api & memory)
        : m_decoder(nullptr)
        , m_loader(nullptr)
        , m_memory(memory)
    {
        m_decoder = decoder::create(game, isa, m_memory);
        if (m_decoder)
        {
            auto loader = loader::create(game, *m_decoder, m_memory);
            if (loader && loader->load())
            {
                m_loader = loader;
            }
            else
            {
                delete loader;
            }
        }
    }

    analyzer::~analyzer(void)
    {
        delete m_loader;
        m_loader = nullptr;
        delete m_decoder;
        m_decoder = nullptr;
    }

    // TODO: cache ?
    auto analyzer::analyze_instruction(std::uint32_t address, instruction & ins) -> bool
    {
        assert(m_decoder);
        auto const size = m_decoder->decode_instruction(address, ins);
        if (! size)
        {
            return false;
        }
        return true;
    }

    auto analyzer::analyze_instruction(insn_t & insn) -> bool
    {
        instruction src = {};
        if (! analyze_instruction(insn.ea, src))
            return false;
        IDASCM_LOG_T("analyze +0x%08x 0x%04x %s", insn.ea, src.opcode, instruction_name(src).data());
        if (! handle_instruction(src, insn))
            return false;
        return true;
    }

    auto analyzer::handle_instruction(instruction const & src, insn_t & insn) const -> bool
    {
        insn.itype = src.opcode;
        for (std::uint8_t i = 0; i < src.operand_count; ++ i)
        {
            if (i >= std::size(insn.ops))
            {
                IDASCM_LOG_W("%s: too many operands (%d)", instruction_name(src).data(), src.operand_count);
                break;
            }
            handle_operand(src, i, insn.ops[i]);
        }
        insn.size = src.size;
        insn_set_inversion_flag(insn, (src.flags & instruction_flag_not) != 0);
        insn_set_operand_count(insn, src.operand_count);
        return true;
    }

    auto analyzer::handle_operand(instruction const & src, std::uint8_t index, op_t & dst) const -> bool
    {
        assert(index < std::size(src.operand_list));
        if (index >= src.operand_count)
            return false;
        auto const command = src.command;
        if (! command)
            return false;
        dst.offb = static_cast<char>(src.operand_list[index].offset);
        dst.flags |= OF_SHOW;
        op_set_type(dst, src.operand_list[index].type);
        op_set_value(dst, src.operand_list[index].value);

        if (index < command->arguments.size())
        {
            // logical types
            switch (command->arguments[index].type)
            {
                case type::address:
                {
                    std::int32_t value = 0;
                    if (to_int(src.operand_list[index], value))
                    {
                        if (value >= 0)
                        {
                            dst.type    = o_far;
                            dst.dtype   = dt_code;
                            dst.addr    = value;
                        }
                        else
                        {
                            dst.type    = o_near;
                            dst.dtype   = dt_code;
                            dst.addr    = -value;
                        }
                        return true;
                    }
                    // TODO: handle non-integer address representation
                    break;
                }
                case type::mission:
                {
                    std::int32_t value = 0;
                    if (to_int(src.operand_list[index], value))
                    {
                        if (value < 0)
                            break;
                        if (! m_loader)
                            break;
                        auto const address = m_loader->get_mission_descriptor_address(value);
                        if (address == -1)
                        {
                            IDASCM_LOG_W("Invalid mission index: %d", value);
                            break;
                        }
                        dst.type    = o_mem;
                        dst.dtype   = dt_dword;
                        dst.addr    = address;
                        return true;
                    }
                    break;
                }
                case type::script:
                {
                    std::int32_t value = 0;
                    if (to_int(src.operand_list[index], value))
                    {
                        // TODO
                    }
                    break;
                }
                case type::model:
                {
                    std::int32_t value = 0;
                    if (to_int(src.operand_list[index], value))
                    {
                        if (value < 0)
                        {
                            // Local script identifier (index)
                            if (! m_loader)
                                break;
                            auto const address = m_loader->get_object_descriptor_address(-value);
                            if (address == -1)
                            {
                                IDASCM_LOG_W("Invalid local model index: %d", value);
                                break;
                            }
                            dst.type    = o_mem;
                            dst.dtype   = dt_string;
                            dst.addr    = address;
                            return true;
                        }
                        else
                        {
                            // Global game identifier
                        }
                    }
                    break;
                }
            }
        }

        // actual types
        switch (src.operand_list[index].type)
        {
            case operand_type::none:
            {
                dst.type    = o_void;
                dst.dtype   = dt_void;
                dst.clr_shown();
                break;
            }
            case operand_type::string8:
            {
                dst.type    = o_imm;
                dst.dtype   = dt_string;
                dst.addr    = src.address + src.operand_list[index].offset + src.operand_list[index].size - 8u;
                break;
            }
            case operand_type::string:
            case operand_type::string16:
            case operand_type::string128:
            {
                dst.type    = o_imm;
                dst.dtype   = dt_string;
                dst.addr    = src.operand_list[index].value.string.address;
                break;
            }
            case operand_type::int0:
            {
                dst.type    = o_imm;
                dst.dtype   = dt_dword;
                dst.value   = 0;
                break;
            }
            case operand_type::int8:
            {
                dst.type    = o_imm;
                dst.dtype   = dt_byte;
                break;
            }
            case operand_type::int16:
            {
                dst.type    = o_imm;
                dst.dtype   = dt_word;
                break;
            }
            case operand_type::int32:
            {
                dst.type    = o_imm;
                dst.dtype   = dt_dword;
                break;
            }
            case operand_type::float0:
            {
                dst.type    = o_imm;
                dst.dtype   = dt_float;
                break;
            }
            case operand_type::float8p:
            case operand_type::float16p:
            case operand_type::float16i:
            case operand_type::float24p:
            {
                dst.type    = o_imm;
                dst.dtype   = dt_packreal;
                break;
            }
            case operand_type::float32:
            {
                dst.type    = o_imm;
                dst.dtype   = dt_float;
                break;
            }
            case operand_type::global:
            {
                dst.type    = o_mem;
                dst.dtype   = dt_dword;
                dst.addr    = src.operand_list[index].value.address;
                break;
            }
            case operand_type::local:
            {
                dst.type    = o_reg;
                dst.dtype   = dt_dword;
                dst.reg     = src.operand_list[index].value.address;
                break;
            }
            case operand_type::global_array:
            {
                dst.type    = o_displ;
                dst.dtype   = dt_dword;
                dst.reg     = src.operand_list[index].value.array.index;
                dst.addr    = src.operand_list[index].value.array.address;
                break;
            }
            case operand_type::local_array:
            {
                dst.type    = o_phrase;
                dst.dtype   = dt_dword;
                dst.reg     = src.operand_list[index].value.array.index;
                dst.addr    = src.operand_list[index].value.array.address;
                break;
            }
            default:
            {
                dst.flags &= ~OF_SHOW;
                IDASCM_LOG_W("unsupported operand type: %d", src.operand_list[index].type);
                break;
            }
        }
        return true;
    }
}
