# pragma once
# include <ida/base/base.hpp>
# include <engine/memory.hpp>

namespace idascm
{
    class memory_linput : public memory_device
    {
        public:
            virtual auto read(std::uint32_t address, void * dst, std::uint32_t size) -> std::uint32_t override;

        public:
            explicit memory_linput(linput_t * li)
                : memory_device()
                , m_li(li)
            {}

        private:
            linput_t * m_li;
    };
}
