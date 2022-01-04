# pragma once
# include <string_view>

namespace idascm
{
    enum class json_type
    {
        invalid,
        object,
        array,
        primitive,
    };

    class json_value
    {
        public:
            static auto from_string(char const * string) -> json_value;
            static auto from_string(char const * string, std::size_t length, int * error_code = nullptr) -> json_value;
            static auto from_file(char const * path) -> json_value;

        public:
            auto type(void) const noexcept -> json_type;
            auto is_valid(void) const noexcept -> bool
            {
                return nullptr != m_data;
            }
            auto contains(char const * key) const noexcept -> bool;
            auto key_at(std::size_t index) const -> json_value;
            auto at(std::size_t index) const -> json_value;
            auto at(char const * key) const -> json_value;
            auto at(char const * key, std::size_t length) const -> json_value;
            auto size(void) const noexcept -> std::size_t;
            auto to_string(void) const noexcept -> std::string_view;
            auto c_str(void) const noexcept -> char const *;

        public:
            auto operator [] (char const * key) const -> json_value
            {
                return at(key);
            }

            auto operator [] (std::size_t index) const -> json_value
            {
                return at(index);
            }

            auto operator = (json_value const & other) -> json_value &
            {
                assign(other.m_data, other.m_begin, other.m_end);
                return *this;
            }

        public:
            json_value(void)
                : m_data(nullptr)
                , m_begin(0)
                , m_end(0)
            {}

            json_value(json_value const & other)
                : json_value()
            {
                assign(other.m_data, other.m_begin, other.m_end);
            }

            ~json_value(void)
            {
                release();
            }

        protected:
            void assign(struct json_data * data, std::size_t begin, std::size_t end);
            void release(void);

        private:
            struct json_data *  m_data;
            std::size_t         m_begin;
            std::size_t         m_end;
    };
}
