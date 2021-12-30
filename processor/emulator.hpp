# pragma once
# include <processor/processor.hpp>

namespace idascm
{
    // class analyzer;
    class command_set;

    class emulator
    {
        public:
            auto emulate(insn_t const & insn) -> int;
            void emulate_operand(insn_t const & insn, op_t const & op);
            void set_isa(command_set const * isa);

        public:
            emulator(void)
                : m_isa(nullptr)
            {}

        protected:
            command_set const * m_isa;
            // analyzer const * m_analyzer;
    };
}
