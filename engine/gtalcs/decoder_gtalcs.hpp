# pragma once
# include <engine/decoder.hpp>

namespace idascm
{
    class decoder_gtalcs : public decoder
    {
        public:
            // Based on re3/lcs definition
            enum class value_type : std::uint8_t
            {
                end,
                int0,
                float0,
                float8,
                float16,
                float24,
                int32,
                int8,
                int16,
                float32,
                timer_first,
                timer_last           = value_type::timer_first        +  2 - 1,
                local_first,
                local_last           = value_type::local_first        + 96 - 1,
                local_array_first,
                local_array_last     = value_type::local_array_first  + 96 - 1,
                global_first,
                global_last          = value_type::global_first       + 26 - 1,
                global_array_first,
                global_array_last    = value_type::global_array_first + 26 - 1,
            };

        public:
            static constexpr auto to_uint(value_type type) noexcept -> std::uint8_t
            {
                return static_cast<std::uint8_t>(type);
            }

            static auto to_operand_type(value_type src) noexcept -> operand_type;

        public:
            virtual auto decode_operand_type(std::uint32_t address, operand_type & type) const -> std::uint32_t override;
            virtual auto decode_operand_value(std::uint32_t address, operand_type type, operand_value & value) const -> std::uint32_t override;
            virtual auto decode_operand(std::uint32_t address, operand & op) const -> std::uint32_t override;
    };
}
