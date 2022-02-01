# pragma once
# include <engine/engine.hpp>

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
            virtual ~decoder(void) noexcept;
            virtual auto decode_instruction(std::uint32_t address, instruction & in) const -> std::uint32_t;
            virtual auto decode_operand(std::uint32_t address, operand & op) const -> std::uint32_t = 0;

            void set_command_set(command_set const * isa)
            {
                m_isa = isa;
            }

            void set_memory_api(memory_api * api)
            {
                m_memory = api;
            }

        protected:
            decoder(void);
            virtual auto decode_operand_value(std::uint32_t address, operand_type type, operand_value & value) const -> std::uint32_t;

        protected:
            command_set const * m_isa;
            memory_api *        m_memory;
    };
}
