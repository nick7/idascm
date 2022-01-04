# pragma once
# include <string_view>

namespace idascm
{
    class json_array;
    class json_object;
    class json_primitive;
    class json_value;

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

            auto to_primitive(void) const -> json_primitive;
            auto to_array(void) const -> json_array;
            auto to_object(void) const -> json_object;

        public:
            auto operator = (json_value const & other) -> json_value &
            {
                assign(other);
                return *this;
            }

            auto operator = (json_value && other) -> json_value &
            {
                swap(other);
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

            json_value(json_value && other) noexcept
                : json_value()
            {
                swap(other);
            }

            ~json_value(void) noexcept
            {
                release();
            }

        protected:
            void assign(struct json_data * data, std::size_t begin, std::size_t end) noexcept;
            void assign(json_value const & other) noexcept
            {
                if (this != &other)
                    assign(other.m_data, other.m_begin, other.m_end);
            }
            void release(void) noexcept;
            void swap(json_value & other) noexcept;

        protected:
            struct json_data *  m_data;
            std::size_t         m_begin;
            std::size_t         m_end;

        friend json_primitive;
        friend json_array;
        friend json_object;
    };

    class json_primitive : public json_value
    {
        public:
            auto to_string(void) const noexcept -> std::string_view;
            auto c_str(void) const noexcept -> char const *;

        public:
            auto operator = (json_primitive const & other) noexcept -> json_primitive &
            {
                assign(other);
                return *this;
            }

            auto operator = (json_primitive && other) noexcept -> json_primitive &
            {
                swap(other);
                return *this;
            }

        public:
            json_primitive(void) = default;
            json_primitive(json_primitive const & other) = default;
            json_primitive(json_primitive && other) = default;
    };

    class json_array : public json_value
    {
        public:
            auto at(std::size_t index) const -> json_value;
            auto size(void) const noexcept -> std::size_t;

        public:
            auto operator [] (std::size_t index) const -> json_value
            {
                return at(index);
            }

            auto operator = (json_array const & other) noexcept -> json_array &
            {
                assign(other);
                return *this;
            }

            auto operator = (json_array && other) noexcept -> json_array &
            {
                swap(other);
                return *this;
            }

        public:
            json_array(void) = default;

            json_array(json_array const & other)
                : json_value(other)
            {}

            json_array(json_array && other) noexcept
                : json_value(other)
            {}
    };

    class json_object : public json_value
    {
        public:
            auto contains(char const * key) const noexcept -> bool;
            auto key_at(std::size_t index) const -> json_value;
            auto at(std::size_t index) const -> json_value;
            auto at(char const * key) const -> json_value;
            auto at(char const * key, std::size_t length) const -> json_value;
            auto size(void) const noexcept -> std::size_t;

        public:
            auto operator [] (char const * key) const -> json_value
            {
                return at(key);
            }

            auto operator = (json_object const & other) noexcept -> json_object &
            {
                assign(other);
                return *this;
            }

            auto operator = (json_object && other) noexcept -> json_object &
            {
                swap(other);
                return *this;
            }

        public:
            json_object(void) = default;
            json_object(json_object const & object) = default;
            json_object(json_object && object) noexcept = default;
    };
}
