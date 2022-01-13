# pragma once
# include <ida/base/base.hpp>
# include <engine/memory.hpp>

namespace idascm
{
    class memory_api_ida : public memory_api
    {
        virtual auto read(std::uint32_t address, void * dst, std::uint32_t size) -> std::uint32_t override
        {
            return get_bytes(dst, size, address);
        }
    };
}
