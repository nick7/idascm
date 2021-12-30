# pragma once
# include <processor/processor.hpp>

namespace idascm
{
    class command_set;
    class decoder;
    class memory_api_ida;

    class analyzer
    {
        public:
            void set_isa(command_set const * set);
            auto analyze(insn_t * insn) -> int;

        public:
            analyzer(void);
            ~analyzer(void);

        protected:
            decoder *           m_decoder;
            memory_api_ida *    m_memory;
    };
}
