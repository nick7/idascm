# include <engine/memory.hpp>
# include <cassert>

namespace idascm
{
    // virtual
    auto memory_api_buffer::read(std::uint32_t offset, void* dst, std::uint32_t size) -> std::uint32_t
    {
        auto left = static_cast<std::uint32_t>(std::min<std::size_t>(m_size - offset, size));
        if (left)
            std::memcpy(dst, m_memory + offset, left);
        return left;
    }

    // virtual
    auto memory_api_stdio::read(std::uint32_t offset, void* dst, std::uint32_t size) -> std::uint32_t
    {
        assert(m_stream);
        if (offset != std::fseek(m_stream, offset, SEEK_SET))
            return 0;
        return std::fread(dst, 1, size, m_stream);
    }
    
    memory_api_stdio::~memory_api_stdio(void)
    {
        if (m_stream)
        {
            if (m_is_owner)
                std::fclose(m_stream);
            m_stream = nullptr;
        }
    }
}
