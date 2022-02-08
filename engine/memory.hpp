# pragma once
# include <engine/engine.hpp>

namespace idascm
{
    // memory access interface
    class memory_device
    {
        public:
            virtual auto read(std::uint32_t address, void * dst, std::uint32_t size) -> std::uint32_t = 0;

        public:
            virtual ~memory_device(void) = default;

        protected:
            memory_device(void) = default;
            memory_device(memory_device const &) = delete;
            memory_device & operator = (memory_device const &) = delete;
    };

    class memory_buffer : public memory_device
    {
        public:
            virtual auto read(std::uint32_t offset, void * dst, std::uint32_t size)->std::uint32_t override;

        public:
            explicit memory_buffer(void * memory, std::size_t size)
                : memory_device()
                , m_memory(static_cast<std::uint8_t *>(memory))
                , m_size(size)
            {}

        private:
            std::uint8_t *  m_memory;
            std::size_t     m_size;
    };

    class memory_stdio : public memory_device
    {
        public:
            virtual auto read(std::uint32_t offset, void* dst, std::uint32_t size) -> std::uint32_t override;

        public:
            explicit memory_stdio(std::FILE * file, bool is_owner);
            memory_stdio(memory_stdio const &) = delete;
            memory_stdio(memory_stdio && other) noexcept
                : memory_device()
                , m_stream(nullptr)
                , m_is_owner(false)
            {
                swap(other);
            }
            auto operator = (memory_stdio const &) -> memory_stdio & = delete;
            auto operator = (memory_stdio && other) noexcept -> memory_stdio &
            {
                swap(other);
            }
            virtual ~memory_stdio(void) override;

        protected:
            void swap(memory_stdio & other) noexcept
            {
                std::swap(m_stream,   other.m_stream);
                std::swap(m_is_owner, other.m_is_owner);
            }

        private:
            std::FILE * m_stream;
            bool        m_is_owner;
    };

    // Reading helper
    class memory_reader
    {
        public:
            explicit memory_reader(memory_device & mem, std::uint32_t pointer)
                : m_memory(mem)
                , m_pointer(0)
            {
                set_pointer(pointer);
            }

            auto set_pointer(std::uint32_t pointer) noexcept -> bool
            {
                m_pointer = pointer;
                return true;
            }

            auto pointer(void) const noexcept -> std::uint32_t
            {
                return m_pointer;
            }

            auto skip(std::uint32_t size) noexcept -> bool
            {
                return set_pointer(pointer() + size);
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

        private:
            memory_device & m_memory;
            std::uint32_t   m_pointer;
    };

    template <typename type>
    auto operator >> (memory_reader & reader, type & dst)
    {
        reader.read(dst);
        return reader;
    }
}
