# include <core/json.hpp>
# include <algorithm>
# include <cassert>
# include <cstring>
# include <3rd-party/jsmn/jsmn.h>

namespace idascm
{
    struct json_data
    {
        char *          source;     // input source as is
        std::size_t     length;     // input source length
        std::size_t     refs;       // references
        std::size_t     count;      // token pool usage
        std::size_t     capacity;   // token pool max size
        jsmntok_t *     tokens;     // token pool
    };

    namespace
    {
        void json_data_release(json_data * data)
        {
            if (-- data->refs)
                return;
            delete [] data->source;
            delete [] data->tokens;
            delete data;
        }

        auto json_data_create(std::size_t length, std::size_t capacity) -> json_data *
        {
            auto data = new (std::nothrow) json_data;
            if (! data)
                return nullptr;
            data->refs              = 1;
            data->source            = new (std::nothrow) char[length + 1];
            data->tokens            = new (std::nothrow) jsmntok_t[capacity];
            if (! data->source || ! data->tokens)
            {
                json_data_release(data);
                return nullptr;
            }
            data->length            = length;
            data->source[0]         = '\0';
            data->source[length]    = '\0';
            data->capacity          = capacity;
            data->count             = 0;
            return data;
        }

        auto unescape_string(char * string) -> int
        {
            assert(string);
            char * dst = string;
            char * src = string;
            while (*dst)
            {
                if (*dst == '\\')
                {
                    ++ dst;
                    switch (*dst)
                    {
                        case 'n':
                            *src++ = '\n';
                            break;
                        case 'r':
                            *src++ = '\r';
                            break;
                        case 't':
                            *src++ = '\t';
                            break;
                        case '"':
                            *src++ = '"';
                            break;
                        default:
                            *src++ = 0;
                            break;
                    }
                    ++ dst;
                }
                else
                {
                    *src++ = *dst++;
                }
            }
            *src++ = '\0';
            return (int) (src - string) - 1;
        }

        void fix_strings(json_data * data)
        {
            for (std::size_t i = 0; i < data->count; ++ i)
            {
                switch (data->tokens[i].type)
                {
                    case JSMN_PRIMITIVE:
                    case JSMN_STRING:
                    {
                        char * string = &data->source[data->tokens[i].start];
                        data->source[data->tokens[i].end] = '\0';
                        // if (data->tokens[i].type == JSMN_STRING)
                        data->tokens[i].end = data->tokens[i].start + unescape_string(string);
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }
            }
        }

        auto token_is_equal(json_data const * data, jsmntok_t const * token, char const * string, std::size_t length) -> bool
        {
            assert(data && string && token);
            if (token->end - token->start != length)
                return false;
            return 0 == strncmp(string, data->source + token->start, length);
        }

        auto token_string(json_data const * data, jsmntok_t const * token, char * dst, std::size_t size) -> std::size_t
        {
            assert(data && token && dst);
            auto const length = static_cast<std::size_t>(token->end - token->start);
            size = std::min(size, length);
            std::memcmp(dst, data->source + token->start, size);
            return length;
        }

        auto token_size(json_data const * data, jsmntok_t const * token) -> std::size_t
        {
            assert(data && token);
            int left = token->size;
            ++ token;
            std::size_t size = 1;
            while (left)
            {
                left += token->size;
                size += 1;
                ++ token;
                -- left;
            }
            return size;
        }

        auto token_find(json_data const * data, jsmntok_t const * root, char const * key, std::size_t length) -> jsmntok_t const *
        {
            assert(data && root && key);
            if (root->type == JSMN_OBJECT)
            {
                jsmntok_t const * p = root + 1;
                for (int i = 0 ; i < root->size; ++i)
                {
                    if (p->type == JSMN_STRING)
                    {
                        if (token_is_equal(data, p, key, length))
                            return p;
                    }
                    p += token_size(data, p);
                }
            }
            return nullptr;
        }

        auto token_at(json_data const * data, jsmntok_t const * root, std::size_t index) -> jsmntok_t const * 
        {
            assert(data && root);
            jsmntok_t const * p = root + 1;
            switch (root->type)
            {
                case JSMN_OBJECT:
                    for (std::size_t i = 0 ; i < static_cast<std::size_t>(root->size); ++ i)
                    {
                        if (p->type == JSMN_STRING)
                        {
                            if (i == index)
                                return p;
                        }
                        p += token_size(data, p);
                    }
                    break;
                case JSMN_ARRAY:
                    for (std::size_t i = 0 ; i < static_cast<std::size_t>(root->size); ++ i)
                    {
                        if (i == index)
                            return p;
                        p += token_size(data, p);
                    }
                    break;
                default:
                    break;
            }
            return nullptr;
        }

        auto file_contents(char const * path) -> std::string
        {
            std::string contents;
            if (auto stream = std::fopen(path, "rb"))
            {
                std::fseek(stream, 0, SEEK_END);
                auto const length = std::ftell(stream);
                std::fseek(stream, 0, SEEK_SET);
                contents.resize(length);
                std::fread(&contents[0], 1, length, stream);
                std::fclose(stream);
            }
            return contents;
        }
    }

    // static
    // TODO: error handling
    auto json_value::from_string(char const * string) -> json_value
    {
        return from_string(string, string ? std::strlen(string) : 0);
    }

    // static
    auto json_value::from_string(char const * string, std::size_t length, int * error_code) -> json_value
    {
        std::size_t capacity = 128;
        while (true)
        {
            json_data * data = json_data_create(length, capacity);
            if (! data)
            {
                if (error_code)
                    *error_code = JSMN_ERROR_NOMEM;
                break;
            }
            std::memcpy(data->source, string, length + 1);

            jsmn_parser parser;
            jsmn_init(&parser);
            int result = jsmn_parse(&parser, data->source, data->length, data->tokens, (unsigned int) data->capacity);
            if (result >= 0)
            {
                data->count = result;
                json_value value;
                value.m_data    = data;
                value.m_begin   = 0;
                value.m_end     = data->count;
                fix_strings(data);
                if (error_code)
                    *error_code = 0;
                return value;
            }
            json_data_release(data);
            switch (result)
            {
                case JSMN_ERROR_NOMEM:
                    capacity <<= 1;
                    continue;
            }
            if (error_code)
                *error_code = result;
            break;
        }
        return {};
    }

    // static
    // TODO: error handling
    auto json_value::from_file(char const * path) -> json_value
    {
        auto const contents = file_contents(path);
        return from_string(contents.c_str(), contents.length());
    }

    auto json_value::type(void) const noexcept -> json_type
    {
        if (m_data)
        {
            switch (m_data->tokens[m_begin].type)
            {
                case JSMN_UNDEFINED:
                    return json_type::invalid;
                case JSMN_OBJECT:
                    return json_type::object;
                case JSMN_ARRAY:
                    return json_type::array;
                case JSMN_STRING:
                case JSMN_PRIMITIVE:
                    return json_type::primitive;
            }
        }
        return json_type::invalid;
    }

    auto json_value::contains(char const * key) const noexcept -> bool
    {
        return m_data && token_find(m_data, &m_data->tokens[m_begin], key, key ? std::strlen(key) : 0);
    }

    auto json_value::key_at(std::size_t index) const -> json_value
    {
        json_value value;
        if (m_data && JSMN_OBJECT == m_data->tokens[m_begin].type)
        {
            if (auto const token = token_at(m_data, &m_data->tokens[m_begin], index))
            {
                value.assign(m_data, token - m_data->tokens, token + token_size(m_data, token) - m_data->tokens);
            }
        }
        return value;
    }

    auto json_value::at(std::size_t index) const -> json_value
    {
        json_value value;
        if (m_data)
        {
            switch (m_data->tokens[m_begin].type)
            {
                case JSMN_OBJECT:
                    if (auto const token = token_at(m_data, &m_data->tokens[m_begin], index))
                    {
                        auto const next = token + 1;
                        value.assign(m_data, next - m_data->tokens, next + token_size(m_data, next) - m_data->tokens);
                    }
                    break;
                case JSMN_ARRAY:
                    if (auto const token = token_at(m_data, &m_data->tokens[m_begin], index))
                    {
                        value.assign(m_data, token - m_data->tokens, token + token_size(m_data, token) - m_data->tokens);
                    }
                    break;
            }
        }
        return value;
    }

    auto json_value::at(char const * key) const -> json_value
    {
        return at(key, key ? std::strlen(key) : 0);
    }

    auto json_value::at(char const * key, std::size_t length) const -> json_value
    {
        json_value value;
        if (m_data)
        {
            if (auto const token = token_find(m_data, &m_data->tokens[m_begin], key, length))
            {
                auto const next = token + 1;
                value.assign(m_data, next - m_data->tokens, next + token_size(m_data, next) - m_data->tokens);
            }
        }
        return value;
    }

    auto json_value::size(void) const noexcept -> std::size_t
    {
        if (m_data)
        {
            switch (m_data->tokens[m_begin].type)
            {
                case JSMN_OBJECT:
                case JSMN_ARRAY:
                    return static_cast<std::size_t>(m_data->tokens[m_begin].size);
                default:
                    break;
            }
        }
        return 0;
    }

    auto json_value::to_string(void) const noexcept -> std::string_view
    {
        if (m_data)
        {
            switch (m_data->tokens[m_begin].type)
            {
                case JSMN_STRING:
                case JSMN_PRIMITIVE:
                    return std::string_view(m_data->source + m_data->tokens[m_begin].start);
                default:
                    break;
            }
        }
        return {};
    }

    auto json_value::c_str(void) const noexcept -> char const *
    {
        if (m_data)
        {
            switch (m_data->tokens[m_begin].type)
            {
                case JSMN_STRING:
                case JSMN_PRIMITIVE:
                    return m_data->source + m_data->tokens[m_begin].start;
                default:
                    break;
            }
        }
        return nullptr;
    }

    void json_value::assign(json_data * data, std::size_t begin, std::size_t end)
    {
        m_begin = begin;
        m_end   = end;
        if (m_data != data)
        {
            if (data)
                ++ data->refs;
            release();
        }
        m_data  = data;
    }

    void json_value::release(void)
    {
        if (m_data)
            json_data_release(m_data);
        m_data = nullptr;
    }

}
