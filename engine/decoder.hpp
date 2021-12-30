# pragma once
# include <engine/engine.hpp>

namespace idascm
{
    class command_set;
    class memory_api;

    struct instruction;
    struct operand;

    // decoder?
    class decoder
    {
        public:
            auto decode_instruction(std::uint32_t address, instruction & in) const -> std::uint32_t;
            auto decode_operand(std::uint32_t address, operand & op) const -> std::uint32_t;

        public:
            decoder(void)
                : m_isa(nullptr)
                , m_memory(nullptr)
            {}

            void set_command_set(command_set const * isa)
            {
                m_isa = isa;
            }

            void set_memory_api(memory_api * api)
            {
                m_memory = api;
            }

        private:
            command_set const * m_isa;
            memory_api *        m_memory;
    };
}
