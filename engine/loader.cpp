# include <engine/loader.hpp>
# include <engine/decoder/decoder_gta3.hpp>
# include <engine/instruction.hpp>
# include <engine/command.hpp>
# include <engine/memory.hpp>

namespace idascm
{
    namespace
    {
        auto is_goto(command const & cmd) noexcept -> bool
        {
            if (! (cmd.flags & (command_flag_jump | command_flag_stop)))
                return false;
            if (cmd.flags & command_flag_conditional)
                return false;
            if (cmd.argument_count != 1)
                return false;
            return true;
        }

        auto is_goto(instruction const & ins) noexcept -> bool
        {
            if (! ins.command || ! is_goto(*ins.command))
                return false;
            if (ins.operand_count != 1)
                return false;
            return true;
        }
    }

    auto loader::load(void) -> bool
    {
        std::uint8_t header[16];
        if (m_memory_api->read(0, header, sizeof(header)) != sizeof(header))
            return false;
        instruction ins = {};

        std::uint32_t ptr = 0;
        std::int32_t  address = 0;
        if (auto size = m_decoder->decode_instruction(ptr, ins))
        {
            ptr += size;
            // global variables
            if (is_goto(ins) && to_int(ins.operand_list[0], address))
            {
                
                if (auto size = m_decoder->decode_instruction(address, ins))
                {

                }
            }
        }
        return false;


        // if (header[0] == 0x02 && header[1] == 0x00) // GOTO
        // {
        //     if (header[2] == 0x01) // int32
        //     {
        //         return load_gta3();
        //     }
        // }
        // if (header[8 + 0] == 0x02 && header[8 + 1] == 0x00) // GOTO
        // {
        //     if (header[8 + 2] == 0x06) // int32
        //     {
        //         // TODO: implementation
        //         return game::gtalcs;
        //     }
        // }
        // return game::unknown;
    }
}
