# pragma once
# include <ida/utils/utils.hpp>
# include <engine/engine.hpp>
# include <diskio.hpp>
# include <cassert>

namespace idascm
{
    class memory_api_loader : public memory_api
    {
        public:
            virtual auto read(std::uint32_t address, void * dst, std::uint32_t size) -> std::uint32_t override
            {
                assert(m_li);
                if (address != qlseek(m_li, address, SEEK_SET))
                    return 0;
                return qlread(m_li, dst, size);
            }

        public:
            explicit memory_api_loader(linput_t * li)
                : memory_api()
                , m_li(li)
            {}

        private:
            linput_t * m_li;
    };
}
