# pragma once
# include <engine/decoder.hpp>

namespace idascm
{
    enum class operand_type_gtasa : std::uint8_t
    {
        none                    = 0x00,
        int32                   = 0x01,
        global                  = 0x02,
        local                   = 0x03,
        int8                    = 0x04,
        int16                   = 0x05,
        float32                 = 0x06,
        global_array            = 0x07,
        local_array             = 0x08,
        string8                 = 0x09, // known as short string
        global_string8          = 0x0a, // known as short string
        local_string8           = 0x0b, // known as short string
        global_array_string8    = 0x0c, // known as short string
        local_array_string8     = 0x0d, // known as short string
        string                  = 0x0e, // known as pascal string
        string16                = 0x0f, // known as long string
        global_string16         = 0x10, // known as long string
        local_string16          = 0x11, // known as long string
        global_array_string16   = 0x12, // known as long string
        local_array_string16    = 0x13, // known as long string
    };

    auto to_operand_type(operand_type_gtasa internal_type) noexcept -> operand_type;

    class decoder_gtasa : public decoder
    {
        public:
            virtual auto decode_operand(std::uint32_t address, operand & op) const -> std::uint32_t override;

        public:
            decoder_gtasa(command_set const & isa, memory_device & mem)
                : decoder(isa, mem)
            {}

        public:
            auto decode_operand_value(std::uint32_t address, operand_type_gtasa type, operand_value & value) const -> std::uint32_t;
    };
}
