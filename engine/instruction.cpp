# include <engine/instruction.hpp>
# include <engine/command.hpp>

namespace idascm
{
    auto instruction_name(instruction const & ins) noexcept -> char const *
    {
        return ins.command ? ins.command->name : nullptr;
    }
}
