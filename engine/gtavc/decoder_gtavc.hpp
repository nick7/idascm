# pragma once
# include <engine/decoder.hpp>

namespace idascm
{
    enum class operand_type_gtavc : std::uint8_t
    {
        none        = 0x00,
        int32       = 0x01,
        global      = 0x02,
        local       = 0x03,
        int8        = 0x04,
        int16       = 0x05,
        float32     = 0x06,
    };

    auto to_operand_type(operand_type_gtavc internal_type) noexcept -> operand_type;

    class decoder_gtavc : public decoder
    {
        public:
            virtual auto decode_operand(std::uint32_t address, operand & op) const -> std::uint32_t override;

        protected:
            auto decode_operand_value(std::uint32_t address, operand_type_gtavc type, operand_value & value) const -> std::uint32_t;
    };
}
