# pragma once
# include <cstdint>
# include <cstring>
# include <algorithm>

namespace idascm
{
    // virtual machine address
    using vmsize_t = std::uint32_t;

    class memory_api
    {
        public:
            virtual auto read(vmsize_t address, void * dst, vmsize_t size) -> vmsize_t = 0;

            template <typename type>
            auto read(vmsize_t address, type * dst) -> vmsize_t
            {
                return read(address, dst, sizeof(type));
            }
    };

    class memory_api_buffer : public memory_api
    {
        public:
            virtual auto read(vmsize_t offset, void * dst, vmsize_t size) -> vmsize_t override
            {
                auto left = static_cast<vmsize_t>(std::min<std::size_t>(m_size - offset, size));
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
}
