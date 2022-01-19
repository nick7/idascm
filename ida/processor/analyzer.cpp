# include <ida/processor/analyzer.hpp>
# include <engine/command.hpp>
# include <engine/command_set.hpp>
# include <engine/gta3/decoder_gta3.hpp>
# include <engine/gtavc/decoder_gtavc.hpp>
# include <engine/gtasa/decoder_gtasa.hpp>
# include <engine/gtalcs/decoder_gtalcs.hpp>
# include <engine/instruction.hpp>
# include <core/logger.hpp>
# include <cassert>

namespace idascm
{
    class memory_api_ida : public memory_api
    {
        virtual auto read(std::uint32_t address, void * dst, std::uint32_t size) -> std::uint32_t
        {
            return get_bytes(dst, size, address);
        }
    };

    void analyzer::set_isa(command_set const * isa)
    {
        assert(m_decoder);
        m_decoder->set_command_set(isa);
    }

    analyzer::analyzer(game game)
        : m_decoder(nullptr)
        , m_memory(new memory_api_ida)
    {
        switch (game)
        {
            case game::gta3:
                m_decoder = new decoder_gta3;
                break;
            case game::gtavc:
                m_decoder = new decoder_gtavc;
                break;
            case game::gtasa:
                m_decoder = new decoder_gtasa;
                break;
            case game::gtalcs:
                m_decoder = new decoder_gtalcs;
                break;
        }
        assert(m_decoder);
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

    auto analyzer::analyze_instruction(insn_t & insn) -> bool
    {
        instruction src = {};
        if (! analyze_instruction(insn.ea, src))
            return false;
        IDASCM_LOG_D("analyze +0x%08x 0x%04x %s", insn.ea, src.opcode, instruction_name(src).data());
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

        // logical types
        switch (command->argument_list[index])
        {
            case argument_type::address:
            {
                std::int32_t value = 0;
                if (to_int(src.operand_list[index], value))
                {
                    dst.value = *reinterpret_cast<std::uint32_t *>(&value);
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
        }

        // actual types
        switch (src.operand_list[index].type)
        {
            case operand_type::string64:
            {
                dst.type    = o_imm;
                dst.dtype   = dt_string;
                dst.addr    = src.address + src.operand_list[index].offset + src.operand_list[index].size - 8u;
                break;
            }
            case operand_type::string:
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
            case operand_type::float8:
            case operand_type::float16:
            case operand_type::float24:
            case operand_type::float32:
            case operand_type::float16i:
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
            case operand_type::local:
            {
                dst.type    = o_reg;
                dst.dtype   = dt_dword;
                dst.reg     = src.operand_list[index].value.address;
                break;
            }
            // case operand_type::timer:
            // {
            //     dst.type    = o_reg;
            //     dst.type    = dt_dword;
            //     dst.reg     = src.operand_list[index].value_address;
            //     break;
            // }
            default:
            {
                dst.flags &= ~OF_SHOW;
                IDASCM_LOG_W("unsupported operand type: %d", src.operand_list[index].type);
                break;
            }
        }
        op_set_value(dst, src.operand_list[index].value);
        return true;
    }
}
