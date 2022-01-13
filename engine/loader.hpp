# pragma once
# include <engine/engine.hpp>
# include <engine/version.hpp>

namespace idascm
{
    class decoder;
    class memory_api;

    class loader
    {
        public:
            auto load(void) -> bool;
    
        public:
            explicit loader(memory_api * memory, decoder * decoder)
                : m_memory_api(memory)
                , m_decoder(decoder)
            {}
    
        private:
            memory_api *    m_memory_api;
            decoder *       m_decoder;
    };
}
