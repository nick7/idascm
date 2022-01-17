# include <engine/instruction.hpp>
# include <engine/command.hpp>

namespace idascm
{
    namespace
    {
        auto as_float(std::uint32_t src) noexcept -> float
        {
            return *reinterpret_cast<float const *>(&src);
        }
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

    auto name(instruction const & ins) noexcept -> char const *
    {
        return ins.command ? ins.command->name : nullptr;
    }
}
