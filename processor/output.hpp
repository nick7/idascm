# pragma once
# include <processor/processor.hpp>

namespace idascm
{
    struct operand;

    class analyzer;
    class command_set;

    class output
    {
        public:
            auto output_instruction(outctx_t & ctx) -> bool;
            void output_mnemonics(outctx_t & ctx);
            auto output_operand(outctx_t & ctx, op_t const & op) -> bool;

            void set_isa(command_set const * isa)
            {
                m_isa = isa;
            }

            void set_analyzer(analyzer * analyzer)
            {
                m_analyzer = analyzer;
            }

        public:
            output(void)
                : m_isa(nullptr)
                , m_analyzer(nullptr)
            {}

        protected:
            command_set const * m_isa;
            analyzer *          m_analyzer;
    };
}

void idaapi out_insn(outctx_t & ctx);
void idaapi out_mnem(outctx_t & ctx);
bool idaapi out_opnd(outctx_t & ctx, op_t const & x);
