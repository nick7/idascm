# include "analyzer.hpp"
# include <engine/command.hpp>
# include <engine/command_set.hpp>
# include <engine/decoder.hpp>
# include <engine/instruction.hpp>
# include <core/logger.hpp>
# include <cassert>

namespace idascm
{
    class memory_api_ida : public memory_api
    {
        virtual auto read(vmsize_t address, void * dst, vmsize_t size) -> vmsize_t
        {
            return get_bytes(dst, size, address);
        }
    };

    void analyzer::set_isa(command_set const * isa)
    {
        assert(m_decoder);
        m_decoder->set_command_set(isa);
    }

    analyzer::analyzer(void)
        : m_decoder(new decoder)
        , m_memory(new memory_api_ida)
    {
        m_decoder->set_memory_api(m_memory);
    }

    analyzer::~analyzer(void)
    {
        delete m_decoder;
        m_decoder = nullptr;
        delete m_memory;
        m_memory = nullptr;
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

    auto analyzer::analyze_instruction(insn_t & insn) -> int
    {
        instruction src = {};
        if (! analyze_instruction(insn.ea, src))
            return 0;
        IDASCM_LOG_T("analyze %s", src.command->name);
        if (! handle_instruction(src, insn))
        {
            return 0;
        }
        return insn.size;
    }

    auto analyzer::handle_instruction(instruction const & src, insn_t & insn) const -> bool
    {
        insn.itype = src.opcode;
        for (std::uint8_t i = 0; i < src.operand_count; ++ i)
        {
            if (i >= std::size(insn.ops))
            {
                IDASCM_LOG_W("%s: too many operands (%d)", instruction_name(src), src.operand_count);
                break;
            }
            handle_operand(src, i, insn.ops[i]);
        }
        insn.size = src.size;
        insn_set_operand_count(insn, src.operand_count);
        return true;
    }

    auto analyzer::handle_operand(instruction const & src, std::uint8_t index, op_t & dst) const -> bool
    {
        assert(index < std::size(src.operand_list));
        if (index >= src.operand_count)
            return false;
        dst.offb = static_cast<char>(src.operand_list[index].offset);
        dst.flags |= OF_SHOW;
        switch (src.operand_list[index].type)
        {
            case operand_string:
            {
                dst.type    = o_imm;
                dst.dtype   = dt_string;
                dst.addr    = src.operand_list[index].value_ptr;
                IDASCM_LOG_D("string at %x", src.operand_list[index].value_ptr);
                break;
            }
            case operand_int8:
            {
                dst.type    = o_imm;
                dst.dtype   = dt_byte;
                dst.value   = src.operand_list[index].value_int8;
                break;
            }
            case operand_int16:
            {
                dst.type    = o_imm;
                dst.dtype   = dt_word;
                dst.value   = src.operand_list[index].value_int16;
                break;
            }
            case operand_int32:
            {
                if (src.command->argument_list[index] == argument_type::address)
                {
                    dst.type    = o_near;
                    dst.dtype   = dt_code;
                    dst.addr    = src.operand_list[index].value_int32;
                    break;
                }
                dst.type    = o_imm;
                dst.dtype   = dt_dword;
                dst.value   = src.operand_list[index].value_int32;
                break;
            }
            case operand_float32:
            {
                dst.type    = o_imm;
                dst.dtype   = dt_float;
                dst.value   = src.operand_list[index].value_int32;
                break;
            }
            case operand_global:
            {
                dst.type    = o_mem;
                dst.dtype   = dt_dword;
                dst.addr    = src.operand_list[index].value_ptr;
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
