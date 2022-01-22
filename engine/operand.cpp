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
            "float0",
            "float8",
            "float16",
            "float16i",
            "float24",
            "float32",
            "string8",
            "string16",
        };

        auto as_float(std::uint32_t src) noexcept -> float
        {
            return *reinterpret_cast<float const *>(&src);
        }
    }

    auto operand_type_from_json(json_primitive const & value) noexcept -> operand_type
    {
        auto const & string = value.to_string();
        for (std::size_t i = 0; i < std::size(g_operand_type_table); ++i)
            if (g_operand_type_table[i] == string)
                return static_cast<operand_type>(i);
        return operand_type::unknown;
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
