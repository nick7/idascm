# include <ida/base/memory_loader.hpp>
# include <diskio.hpp>
# include <cassert>

namespace idascm
{
    auto memory_api_loader::read(std::uint32_t address, void* dst, std::uint32_t size) -> std::uint32_t
    {
        assert(m_li);
        if (address != qlseek(m_li, address, SEEK_SET))
            return 0;
        return qlread(m_li, dst, size);
    }
}
