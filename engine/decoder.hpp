# pragma once
# include <engine/engine.hpp>
# include <engine/version.hpp>

namespace idascm
{
    class command_set;
    class memory_api;

    struct instruction;
    struct operand;
    struct operand_value;

    enum class operand_type : std::uint8_t;

    class decoder
    {
        public:
            static auto create(game id, command_set const & isa, memory_api & memory) -> decoder *;

        public:
            virtual ~decoder(void) noexcept;
            virtual auto decode_instruction(std::uint32_t address, instruction & in) const -> std::uint32_t;
            virtual auto decode_operand(std::uint32_t address, operand & op) const -> std::uint32_t = 0;
            auto isa(void) const -> command_set const &
            {
                return m_isa;
            }

        protected:
            decoder(command_set const & isa, memory_api & memory);
            virtual auto decode_operand_value(std::uint32_t address, operand_type type, operand_value & value) const -> std::uint32_t;

        protected:
            command_set const & m_isa;
            memory_api &        m_memory;
    };
}
