# pragma once
# include <engine/engine.hpp>
# include <engine/loader.hpp>

namespace idascm
{
    class decoder;

    class loader_gta3 : public loader
    {
        public:
            virtual auto load_header(void) -> bool override;
            virtual auto load_header_layout(void) -> bool override;

        public:
            explicit loader_gta3(memory_device & memory, decoder & decoder)
                : loader(memory)
                , m_decoder(decoder)
            {}

        protected:
            decoder & m_decoder;
    };
}
