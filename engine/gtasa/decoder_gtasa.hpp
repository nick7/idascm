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
        string8                 = 0x09, // aka short string
        global_string8          = 0x0a, // aka short string
        local_string8           = 0x0b, // aka short string
        global_array_string8    = 0x0c, // aka short string
        local_array_string8     = 0x0d, // aka short string
        string                  = 0x0e, // aka pascal string
        string16                = 0x0f, // aka long string
        global_string16         = 0x10, // aka long string
        local_string16          = 0x11, // aka long string
        global_array_string16   = 0x12, // aka long string
        local_array_string16    = 0x13, // aka long string
    };

    constexpr auto to_uint(operand_type_gtasa value) noexcept -> std::uint8_t
    {
        return static_cast<std::uint8_t>(value);
    }

    class decoder_gtasa : public decoder
    {
        public:
            virtual auto decode_operand_type(std::uint32_t address, operand_type & type) const -> std::uint32_t override;
            virtual auto decode_operand_value(std::uint32_t address, operand_type type, operand_value & value) const -> std::uint32_t override;
            virtual auto decode_operand(std::uint32_t address, operand & op) const -> std::uint32_t override;
    };
}
