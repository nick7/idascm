# pragma once
# include <engine/decoder.hpp>

namespace idascm
{
    enum class operand_type_gtasa : std::uint8_t
    {
        none            = 0x00,
        int32           = 0x01,
        global          = 0x02,
        local           = 0x03,
        int8            = 0x04,
        int16           = 0x05,
        float32         = 0x06,
        global_array    = 0x07,
        local_array     = 0x08,
        string64        = 0x09,
    };

    constexpr auto to_uint(operand_type_gtasa value) noexcept -> std::uint8_t
    {
        return static_cast<std::uint8_t>(value);
    }

    class decoder_gtasa : public decoder
    {
        public:
            virtual auto decode_operand_type(std::uint32_t address, operand_type & type) const -> std::uint32_t override;
    };
}
