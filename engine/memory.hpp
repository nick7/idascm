# pragma once
# include <engine/engine.hpp>

namespace idascm
{
    // memory access interface
    class memory_api
    {
        public:
            virtual auto read(std::uint32_t address, void * dst, std::uint32_t size) ->std::uint32_t = 0;

            template <typename type>
            auto read(std::uint32_t address, type * dst) -> std::uint32_t
            {
                return read(address, dst, sizeof(type));
            }

        public:
            virtual ~memory_api(void) = default;

        protected:
            memory_api(void) = default;
            memory_api(memory_api const &) = delete;
            memory_api & operator = (memory_api const &) = delete;
    };

    class memory_api_buffer : public memory_api
    {
        public:
            virtual auto read(std::uint32_t offset, void* dst, std::uint32_t size)->std::uint32_t override;

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

    class memory_api_stdio : public memory_api
    {
        public:
            virtual auto read(std::uint32_t offset, void* dst, std::uint32_t size) -> std::uint32_t override;

        public:
            explicit memory_api_stdio(std::FILE * file, bool is_owner)
                : memory_api()
                , m_stream(file)
                , m_is_owner(is_owner)
            {}

            virtual ~memory_api_stdio(void) override;

        private:
            std::FILE * m_stream;
            bool        m_is_owner;
    };
}
