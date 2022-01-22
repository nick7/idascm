# include <engine/instruction.hpp>
# include <engine/command.hpp>

namespace idascm
{
    auto instruction_name(instruction const & in) noexcept -> std::string_view
    {
        return in.command ? in.command->name : std::string_view();
    }
}
