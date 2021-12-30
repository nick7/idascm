# pragma once
# include <processor/processor.hpp>

namespace idascm
{
    class command_set;

    class output
    {
        public:
            auto instruction(outctx_t & ctx) -> bool;
            void mnemonics(outctx_t & ctx);
            auto operand(outctx_t & ctx, op_t const & op) -> bool;

        public:
            output(void)
                : m_isa(nullptr)
            {}

            void set_isa(command_set const * isa)
            {
                m_isa = isa;
            }

        protected:
            command_set const * m_isa;
    };
}

void idaapi out_insn(outctx_t & ctx);
void idaapi out_mnem(outctx_t & ctx);
bool idaapi out_opnd(outctx_t & ctx, op_t const & x);
