# pragma once
# include <engine/decoder/decoder.hpp>

namespace idascm
{
    class decoder_gta3 : public decoder
    {
        public:
            enum value_type : std::uint8_t
            {
                value_type_none         = 0x00,
                value_type_int32        = 0x01,
                value_type_global       = 0x02,
                value_type_local        = 0x03,
                value_type_int8         = 0x04,
                value_type_int16        = 0x05,
                value_type_float16i     = 0x06,
            };

        public:
            virtual auto decode_operand_type(std::uint32_t address, operand_type & type) const -> std::uint32_t override;
    };
}
