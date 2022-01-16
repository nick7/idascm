# pragma once
# include <engine/engine.hpp>
# include <engine/command.hpp>
# include <engine/version.hpp>
# include <algorithm>

namespace idascm
{
    class json_object;

    // single game implementation
    // opcode database - set of commands
    class command_set
    {
        public:
            auto load(json_object const & object) -> bool;
            auto add_command(std::uint16_t opcode, command const & command) -> bool;
            auto set_parent(command_set const * parent) -> bool;

            auto get_command(std::uint16_t opcode) const noexcept -> command const *
            {
                if (opcode < std::size(m_lookup))
                {
                    if (auto cmd = m_lookup[opcode])
                        return cmd;
                }
                if (m_parent)
                {
                    return m_parent->get_command(opcode);
                }
                return nullptr;
            }

            auto get_version(void) const noexcept -> version
            {
                return m_version;
            }

        public:
            command_set(version ver);
            ~command_set(void);

        private:
            command_set(command_set const &) = delete;
            auto operator = (command_set const &) -> command_set & = delete;

        private:
            command_set const * m_parent;
            version             m_version;
            command *           m_lookup[0x1000];
            command             m_pool[0x1000];
            std::size_t         m_count;
    };
}
