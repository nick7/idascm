# include <engine/command_set.hpp>
# include <core/logger.hpp>
# include <core/json.hpp>

namespace idascm
{
    command_set::command_set(version ver)
        : m_parent(nullptr)
        , m_version(ver)
        , m_count(0)
    {
        std::memset(m_lookup, 0, sizeof(m_lookup));
    }

    command_set::~command_set(void)
    {}

    namespace
    {
        auto opcode_from_string(char const * string) -> std::uint16_t
        {
            if (string[0] == '0' && string[1] == 'x')
                return static_cast<std::uint16_t>(std::strtoul(string, nullptr, 16));
            return static_cast<std::uint16_t>(std::strtoul(string, nullptr, 10));
        }

        auto opcode_from_json(json_value const & value) -> std::uint16_t
        {
            return opcode_from_string(value.to_primitive().c_str());
        }
    }

    auto command_set::load(json_object const & commands) -> bool
    {
        if (! commands.is_valid())
            return false;
        for (std::size_t i = 0; i < commands.size(); ++ i)
        {
            auto const cmd = commands.at(i).to_object();
            if (! cmd.is_valid())
                continue;
            std::uint16_t const opcode = opcode_from_json(commands.key_at(i));
            if (! add_command(opcode, command_from_json(cmd)))
            {
                IDASCM_LOG_W("unable to add command");
            }
        }
        return true;
    }

    auto command_set::add_command(std::uint16_t opcode, command const & command) -> bool
    {
        if (m_count >= std::size(m_pool))
            return false;
        if (opcode >= std::size(m_lookup))
            return false;
        if (m_lookup[opcode])
            return false;
        m_pool[m_count] = command;
        m_lookup[opcode] = &m_pool[m_count];
        ++ m_count;
        return true;
    }

    auto command_set::set_parent(command_set const * parent) -> bool
    {
        if (parent == this)
            return false;
        m_parent = parent;
        return true;
    }
}
