# pragma once
# include <processor/processor.hpp>

namespace idascm
{
    // class analyzer;
    class command_set;

    class emulator
    {
        public:
            auto emulate_instruction(insn_t const & insn) -> bool;
            void emulate_operand(insn_t const & insn, op_t const & op);
            void set_isa(command_set const * isa);

            auto is_return(insn_t const & insn) const -> bool;
            
            auto get_autocomment(insn_t const & insn) const -> qstring;

        public:
            emulator(void)
                : m_isa(nullptr)
            {}

        protected:
            command_set const * m_isa;
            // analyzer const * m_analyzer;
    };
}
