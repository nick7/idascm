# include <engine/command_set.hpp>
# include <engine/command_manager.hpp>
# include <engine/gtavc/decoder_gtavc.hpp>
# include <engine/instruction.hpp>
# include <engine/command_set.hpp>
# include <engine/memory.hpp>
# include <core/json.hpp>
# include <core/logger.hpp>
# include <cassert>

namespace idascm
{
    namespace
    {
        char const gs_commands[] = R"(
        {
            "0x004f": {
                "name": "START_NEW_SCRIPT",
                "args": [ "address", "..." ],
                "flags": [ "call" ],
            },
            "0x03cb": {
                "name": "LOAD_SCENE",
                "args": [ "any", "any", "any" ]
            },
        }
        )";
    }
}

int main(int argc, char * argv[])
{
    using namespace idascm;

    std::uint8_t buffer[] = \
    {
        0x4f, 0x00,             // 004F START_NEW_SCRIPT
        0x01,                   // int32
        0x08, 0x00, 0x00, 0x00, // 0x00000008
        0x04,                   // int8
        0xff,                   // 0xff
        0x05,                   // int16
        0xcd, 0xab,             // 0xabcd
        0x06,                   // float32
        0x00, 0x00, 0x00, 0x00, // 0.f
        0x00,                   // none

        0xcb, 0x03,             // 03CB LOAD_SCENE
        0x06,                   // float32
        0x00, 0x00, 0xa6, 0x42, // 83.0
        0x06,                   // float32
        0x33, 0x73, 0x54, 0xc4, // -849.8
        0x06,                   // float32
        0xcd, 0xcc, 0x14, 0x41, // 9.3
    };
    auto mem = memory_api_buffer(buffer, sizeof(buffer));

    command_set isa(version::gtavc);
    isa.load(json_value::from_string(gs_commands).to_object());

    decoder_gtavc dec;
    dec.set_command_set(&isa);
    dec.set_memory_api(&mem);
    
    std::uint32_t ip = 0;

    instruction ins = {};
    if (ip += dec.decode_instruction(ip, ins))
    {
        assert(ins.opcode == 0x004f);
        assert(ins.operand_count == 4);
    }
    if (ip += dec.decode_instruction(ip, ins))
    {
        assert(ins.opcode == 0x03cb);
        assert(ins.operand_count == 3);
    }
    assert(ip == sizeof(buffer));
    
    return 0;
}
