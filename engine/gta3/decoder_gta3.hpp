# pragma once
# include <engine/decoder.hpp>

namespace idascm
{
    enum class operand_type_gta3 : std::uint8_t
    {
        none        = 0x00,
        int32       = 0x01,
        global      = 0x02,
        local       = 0x03,
        int8        = 0x04,
        int16       = 0x05,
        float16i    = 0x06,
    };

    auto to_operand_type(operand_type_gta3 internal_type) noexcept -> operand_type;

    class decoder_gta3 : public decoder
    {
        public:
            virtual auto decode_operand(std::uint32_t address, operand & op) const -> std::uint32_t override;

        protected:
            auto decode_operand_value(std::uint32_t address, operand_type_gta3 type, operand_value & value) const -> std::uint32_t;
    };
}
