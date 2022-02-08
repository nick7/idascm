# pragma once
# include <ida/base/base.hpp>
# include <engine/memory.hpp>

namespace idascm
{
    class memory_ida : public memory_device
    {
        public:
            virtual auto read(std::uint32_t address, void * dst, std::uint32_t size) -> std::uint32_t override;
    };
}
