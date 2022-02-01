# pragma once
# include <engine/engine.hpp>
# include <engine/loader.hpp>

namespace idascm
{
    class decoder_gtasa;

    class loader_gtasa : public loader
    {
        public:
            virtual auto load_header(void) -> bool override;
            virtual auto load_header_layout(void) -> bool override;

        public:
            explicit loader_gtasa(memory_api & memory, decoder_gtasa & decoder)
                : loader(memory)
                , m_decoder(decoder)
            {}

        protected:
            decoder_gtasa & m_decoder;
    };
}
