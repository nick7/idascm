# include <ida/base/memory_ida.hpp>
# include <cassert>

namespace idascm
{
    auto memory_ida::read(std::uint32_t address, void * dst, std::uint32_t size) -> std::uint32_t
    {
        return get_bytes(dst, size, address);
    }
}
