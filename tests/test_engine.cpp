# include <engine/command_set.hpp>
# include <engine/command_manager.hpp>
# include <engine/decoder.hpp>
# include <engine/instruction.hpp>
// # include <core/json.hpp>
# include <core/logger.hpp>

namespace idascm
{
    command_manager g_manager("./");
}

int main(int argc, char * argv[])
{
    using namespace idascm;

    IDASCM_LOG_W("info: %d", 123);

    std::uint8_t buffer[] = \
    {
        // 0x02, 0x00,
        // 0x01,
        // 0x00, 0x00, 0x00, 0x00
        0xcb, 0x03,
        0x06,
        0x00, 0x00, 0xa6, 0x42,
        0x06,
        0x33, 0x73, 0x54, 0xc4,
        0x06,
        0xcd, 0xcc, 0x14, 0x41,
    };
    auto memory = memory_api_buffer(buffer, sizeof(buffer));

    decoder dec;
    dec.set_command_set(g_manager.get_set(version::gtavc_win32));
    dec.set_memory_api(&memory);
    
    instruction ins = {};
    dec.decode_instruction(0, ins);
    
    return 0;
}
