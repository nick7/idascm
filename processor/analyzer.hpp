# pragma once
# include <processor/processor.hpp>

namespace idascm
{
    struct instruction;
    struct operand;

    class command_set;
    class decoder;
    class memory_api_ida;

    // byte analyzer
    // produces instructions (insn_t)
    class analyzer
    {
        public:
            auto analyze_instruction(std::uint32_t address, instruction & ins) -> bool;
            auto analyze_instruction(insn_t & insn) -> bool;
            
            auto handle_instruction(instruction const & src, insn_t & dst) const -> bool;
            auto handle_operand(instruction const & src, std::uint8_t index, op_t & dst) const -> bool;

            void set_isa(command_set const * isa);

        public:
            explicit analyzer(game game);
            ~analyzer(void);

        protected:
            decoder *           m_decoder;
            memory_api_ida *    m_memory;
    };
}
