# pragma once
# include <ida/processor/processor.hpp>

namespace idascm
{
    class analyzer;
    class command_set;

    class emulator
    {
        public:
            auto emulate_instruction(insn_t const & insn) -> bool;
            void emulate_operand(insn_t const & insn, op_t const & op);
            void set_isa(command_set const * isa);
            void set_analyzer(analyzer * analyzer);

            auto is_return(insn_t const & insn) const -> bool;

        public:
            emulator(void)
                : m_isa(nullptr)
                , m_analyzer(nullptr)
            {}

        protected:
            command_set const * m_isa;
            analyzer *          m_analyzer;
    };
}
