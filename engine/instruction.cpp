# include <engine/instruction.hpp>
# include <engine/command.hpp>

namespace idascm
{
    auto to_int(operand const & op, std::int32_t & value) noexcept -> bool
    {
        switch (op.type)
        {
            case operand_type::int8:
                value = op.value_int8;
                return true;
            case operand_type::int16:
                value = op.value_int16;
                return true;
            case operand_type::int32:
                value = op.value_int32;
                return true;
        }
        return false;
    }

    auto to_float(operand const & op, float & value) noexcept -> bool
    {
        switch (op.type)
        {
            case operand_type::float32:
                value = op.value_float32;
                return true;
            case operand_type::float16i:
                value = op.value_int16 / 16.f;
                return true;
            // case operand_type::int32:
            //     value = op.value_int32;
            //     return true;
        }
        return false;
    }

    auto name(instruction const & ins) noexcept -> char const *
    {
        return ins.command ? ins.command->name : nullptr;
    }
}
