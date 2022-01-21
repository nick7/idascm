# pragma once
# include <core/core.hpp>
# include <algorithm>
# include <cstring>
# include <string_view>

namespace idascm
{
    class memory_api
    {
        public:
            virtual auto read(std::uint32_t address, void * dst, std::uint32_t size) ->std::uint32_t = 0;

            template <typename type>
            auto read(std::uint32_t address, type * dst) -> std::uint32_t
            {
                return read(address, dst, sizeof(type));
            }
    };

    class memory_api_buffer : public memory_api
    {
        public:
            virtual auto read(std::uint32_t offset, void * dst, std::uint32_t size) -> std::uint32_t override
            {
                auto left = static_cast<std::uint32_t>(std::min<std::size_t>(m_size - offset, size));
                if (left)
                    std::memcpy(dst, m_memory + offset, left);
                return left;
            }

        public:
            explicit memory_api_buffer(void * memory, std::size_t size)
                : memory_api()
                , m_memory(static_cast<std::uint8_t *>(memory))
                , m_size(size)
            {}

        private:
            std::uint8_t *  m_memory;
            std::size_t     m_size;
    };

    class memory_reader
    {
        public:
            explicit memory_reader(memory_api & mem, std::uint32_t pointer)
                : m_memory(mem)
                , m_pointer(pointer)
            {}

            auto skip(std::uint32_t size) -> bool
            {
                m_pointer += size;
                return true;
            }

            auto read(void * dst, std::uint32_t size) -> bool
            {
                auto read = m_memory.read(m_pointer, dst, size);
                if (read != size)
                {
                    return false;
                }
                m_pointer += read;
                return true;
            }
            
            template <typename type>
            auto read(type & dst) -> bool
            {
                return read(&dst, sizeof(type));
            }

            template <typename type, std::size_t count>
            auto read(type (& dst)[count]) -> bool
            {
                for (auto & item : dst)
                {
                    if (! read(item))
                        return false;
                }
                return true;
            }

            auto pointer(void) const
            {
                return m_pointer;
            }

        private:
            memory_api &    m_memory;
            std::uint32_t   m_pointer;
    };

    template <typename type>
    auto operator >> (memory_reader & reader, type & dst)
    {
        reader.read(dst);
        return reader;
    }
}
