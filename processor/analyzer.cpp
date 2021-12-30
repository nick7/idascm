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

    void analyzer::set_isa(command_set const * isa)
    {
        assert(m_decoder);
        m_decoder->set_command_set(isa);
    }

    int analyzer::analyze(insn_t * insn)
    {
        if (! insn)
        {
            return 0;
        }

        instruction src = {};
        assert(m_decoder);
        auto const size = m_decoder->decode(insn->ea, src);
        if (! size)
        {
            return 0;
        }
        IDASCM_LOG_T("analyze %s", src.command->name);
        insn->itype = src.opcode;
        for (std::uint8_t i = 0; i < src.operand_count; ++ i)
        {
            if (i >= std::size(insn->ops))
            {
                IDASCM_LOG_W("%s: too many operands (%d)", instruction_name(src), src.operand_count);
                break;
            }
            insn->ops[i].offb = static_cast<char>(src.operand_list[i].offset);
            insn->ops[i].flags |= OF_SHOW;
            switch (src.operand_list[i].type)
            {
                case operand_string:
                {
                    insn->ops[i].type   = o_imm;
                    insn->ops[i].dtype  = dt_string;
                    insn->ops[i].addr   = src.operand_list[i].value_ptr;
                    IDASCM_LOG_D("string at %x", src.operand_list[i].value_ptr);
                    break;
                }
                case operand_int8:
                {
                    insn->ops[i].type  = o_imm;
                    insn->ops[i].dtype = dt_byte;
                    insn->ops[i].value = src.operand_list[i].value_int8;
                    break;
                }
                case operand_int16:
                {
                    insn->ops[i].type  = o_imm;
                    insn->ops[i].dtype = dt_word;
                    insn->ops[i].value = src.operand_list[i].value_int16;
                    break;
                }
                case operand_int32:
                {
                    if (src.command->argument_list[i] == argument_type::address)
                    {
                        insn->ops[i].type  = o_near;
                        insn->ops[i].dtype = dt_code;
                        insn->ops[i].addr  = src.operand_list[i].value_int32;
                        break;
                    }
                    insn->ops[i].type  = o_imm;
                    insn->ops[i].dtype = dt_dword;
                    insn->ops[i].value = src.operand_list[i].value_int32;
                    break;
                }
                case operand_float32:
                {
                    insn->ops[i].type  = o_imm;
                    insn->ops[i].dtype = dt_float;
                    insn->ops[i].value = src.operand_list[i].value_int32;
                    break;
                }
                case operand_global:
                {
                    insn->ops[i].type   = o_mem;
                    insn->ops[i].dtype  = dt_dword;
                    insn->ops[i].addr   = src.operand_list[i].value_ptr;
                    break;
                }
                default:
                {
                    insn->ops[i].flags &= ~OF_SHOW;
                    IDASCM_LOG_W("unsupported operand type: %d", src.operand_list[i].type);
                    break;
                }
            }
        }
        insn->size = static_cast<std::uint16_t>(size);
        insn_set_operand_count(*insn, src.operand_count);
        return insn->size;
    }
}
