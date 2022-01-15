# include <engine/loader.hpp>
# include <engine/memory.hpp>
# include <engine/command_set.hpp>
# include <engine/gtavc/decoder_gtavc.hpp>
# include <engine/instruction.hpp>
# include <core/json.hpp>
# include <cassert>

namespace idascm
{
    namespace
    {
        char const gs_json[] = R"(
        {
            "commands": {
                "0x0002": {
                    "name": "GOTO",
                    "arguments": [ "address" ],
                    "flags": [ "call", "stop" ],
                },
                "0x004e": {
                    "name": "TERMINATE_THIS_SCRIPT",
                    "flags": [ "stop" ],
                },
                "0x03a4": {
                    "name": "SCRIPT_NAME",
                    "arguments": [ "string64" ],
                }
            }
        }
        )";
    }
}

int main(int argc, char * argv[])
{
    using namespace idascm;

    std::uint8_t buffer[] = \
    {
        0x02, 0x00,             // 0002 GOTO
        0x01,                   // int32
        0x0c, 0x00, 0x00, 0x00, // 0x0000000c
        0xcc,                   // (padding)
        0x00, 0x00, 0x00, 0x00, // global variables data

        0x02, 0x00,             // 0002 GOTO
        0x01,                   // int32
        0x18, 0x00, 0x00, 0x00, // 0x00000018
        0xcc,                   // (padding)
        0x00, 0x00,
        0x00, 0x00,

        0x02, 0x00,             // 0002 GOTO
        0x01,                   // int32
        0x30, 0x00, 0x00, 0x00, // 0x00000030
        0xcc,                   // (padding)
        0x32, 0x00, 0x00, 0x00, // int32 main size
        0x02, 0x00, 0x00, 0x00, // int32 largest mission size
        0x01, 0x00,             // int16 mission count
        0x00, 0x00,             // int16
        0x32, 0x00, 0x00, 0x00,

        0x4e, 0x00,             // 004E TERMINATE_THIS_SCRIPT

        0x4e, 0x00,             // 004E TERMINATE_THIS_SCRIPT
    };
    auto mem = memory_api_buffer(buffer, sizeof(buffer));
    // auto mem = memory_api_stdio(std::fopen(argv[1], "rb"), true);

    command_set isa;
    isa.load(json_value::from_string(gs_json).to_object());

    decoder_gtavc dec;
    dec.set_command_set(&isa);
    dec.set_memory_api(&mem);
    
    auto ldr = loader(&mem, &dec);
    ldr.load();
    
    return 0;
}
