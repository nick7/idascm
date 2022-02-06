# pragma once
# include <engine/engine.hpp>
# include <engine/loader.hpp>

namespace idascm
{
    class decoder;

    class loader_gtalcs : public loader
    {
        public:
            virtual auto load_header(void) -> bool override;
            virtual auto load_header_layout(void) -> bool override;

        public:
            explicit loader_gtalcs(memory_api & memory, decoder & decoder)
                : loader(memory)
                , m_decoder(decoder)
            {}

        protected:
            decoder & m_decoder;
    };
}
