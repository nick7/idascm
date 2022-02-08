# pragma once
# include <engine/decoder.hpp>

namespace idascm
{
    enum class constants_vcs : std::uint8_t
    {
        timer_count                 = 2,
        local_block_count           = 96,
        local_array_block_count     = 96,
        global_block_count          = 25,
        global_array_block_count    = 26,
    };

    enum class operand_type_gtavcs : std::uint8_t
    {
        none                = 0x00,
        int0                = 0x01,
        float0              = 0x02,
        float8              = 0x03,
        float16             = 0x04,
        float24             = 0x05,
        int32               = 0x06,
        int8                = 0x07,
        int16               = 0x08,
        float32             = 0x09,
        string              = 0x0a,

        timer_first,
        timer_last          = timer_first           + to_uint(constants_vcs::timer_count) - 1,
        local_first,
        local_last          = local_first           + to_uint(constants_vcs::local_block_count) - 1,
        local_array_first,
        local_array_last    = local_array_first     + to_uint(constants_vcs::local_array_block_count) - 1,
        global_first,
        global_last         = global_first          + to_uint(constants_vcs::global_block_count) - 1,
        global_array_first,
        global_array_last   = global_array_first    + to_uint(constants_vcs::global_array_block_count) - 1,
    };

    auto to_operand_type(operand_type_gtavcs type) noexcept -> operand_type;

    class decoder_gtavcs : public decoder
    {
        public:
            virtual auto decode_operand(std::uint32_t address, operand & op) const -> std::uint32_t override;

        public:
            decoder_gtavcs(command_set const & isa, memory_device & mem)
                : decoder(isa, mem)
            {}

        protected:
            auto decode_operand_value(std::uint32_t address, operand_type_gtavcs type, operand_value & value) const -> std::uint32_t;
    };
}
