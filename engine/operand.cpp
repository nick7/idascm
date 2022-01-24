# include <engine/operand.hpp>
# include <core/json.hpp>

namespace idascm
{
    namespace
    {
        char const * const g_operand_type_table[] = \
        {
            "unknown",
            "none",
            "local",
            "global",
            "timer",
            "local_array",
            "global_array",
            "string",
            "int0",
            "int8",
            "int16",
            "int32",
            "int64",
            "uint8",
            "uint16",
            "uint32",
            "uint64",
            "float0",
            "float8",
            "float16",
            "float16i",
            "float24",
            "float32",
            "float64",
            "string8",
            "string16",
        };

        auto as_float(std::uint32_t src) noexcept -> float
        {
            return *reinterpret_cast<float const *>(&src);
        }
    }

    auto operand_type_from_string(char const * string) noexcept -> operand_type
    {
        if (string && string[0])
        {
            for (std::size_t i = 0; i < std::size(g_operand_type_table); ++i)
                if (0 == std::strcmp(g_operand_type_table[i], string))
                    return static_cast<operand_type>(i);
        }
        return operand_type::unknown;
    }

    auto operand_type_to_string(operand_type type) noexcept -> char const *
    {
        if (to_uint(type) < std::size(g_operand_type_table))
            return g_operand_type_table[to_uint(type)];
        return nullptr;
    }

    auto operand_type_from_json(json_primitive const & value) noexcept -> operand_type
    {
        return operand_type_from_string(value.c_str());
    }

    auto operand_type_suffix(operand_type type) noexcept -> char const *
    {
        switch (type)
        {
            case operand_type::int0:
                return "i0";
            case operand_type::int8:
                return "i8";
            case operand_type::int16:
                return "i16";
            case operand_type::int32:
                return "i32";
            case operand_type::int64:
                return "i64";
            case operand_type::float0:
                return "f0";
            case operand_type::float8:
                return "f8";
            case operand_type::float16:
                return "f16";
            case operand_type::float16i:
                return "f16i";
            case operand_type::float24:
                return "f24";
            case operand_type::float32:
                return "f32";
        }
        return nullptr;
    }

    auto operand_type_is_signed(operand_type type) noexcept -> bool
    {
        switch (type)
        {
            case operand_type::int0:
            case operand_type::int16:
            case operand_type::int32:
            case operand_type::int64:
            case operand_type::float0:
            case operand_type::float8:
            case operand_type::float16:
            case operand_type::float16i:
            case operand_type::float24:
            case operand_type::float32:
                return true;
        }
        return false;
    }

    auto to_int(operand const & op, std::int32_t & value) noexcept -> bool
    {
        switch (op.type)
        {
            case operand_type::int8:
                value = op.value.int8;
                return true;
            case operand_type::int16:
                value = op.value.int16;
                return true;
            case operand_type::int32:
                value = op.value.int32;
                return true;
        }
        return false;
    }

    auto to_float(operand_type type, operand_value const & src, float & dst) noexcept -> bool
    {
        switch (type)
        {
            case operand_type::float32:
                dst = src.float32;
                return true;
            case operand_type::float16i:
                dst = src.int16 / 16.f;
                return true;
            case operand_type::float24:
                dst = as_float(src.uint32 << 8u);
                return true;
            case operand_type::float16:
                dst = as_float(src.uint16 << 16u);
                return true;
            case operand_type::float8:
                dst = as_float(src.uint8 << 24u);
                return true;
        }
        return false;
    }

    auto to_float(operand const & op, float & value) noexcept -> bool
    {
        return to_float(op.type, op.value, value);
    }
}
