# pragma once
# include <engine/decoder.hpp>

namespace idascm
{
    // Based on re3/lcs definition
    enum class operand_type_gtalcs : std::uint8_t
    {
        none,
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
        timer_last           = timer_first        +  2 - 1,
        local_first,
        local_last           = local_first        + 96 - 1,
        local_array_first,
        local_array_last     = local_array_first  + 96 - 1,
        global_first,
        global_last          = global_first       + 26 - 1,
        global_array_first,
        global_array_last    = global_array_first + 26 - 1,
    };

    auto to_operand_type(operand_type_gtalcs type) noexcept -> operand_type;

    class decoder_gtalcs : public decoder
    {
        public:
            virtual auto decode_operand(std::uint32_t address, operand & op) const -> std::uint32_t override;

        protected:
            auto decode_operand_value(std::uint32_t address, operand_type_gtalcs type, operand_value & value) const -> std::uint32_t;
    };
}
