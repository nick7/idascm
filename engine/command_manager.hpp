# pragma once
# include <engine/version.hpp>
# include <algorithm>

namespace idascm
{
    struct command;
    class command_set;

    // packed command set manager
    class command_manager
    {
        public:
            auto get_set(version impl) noexcept -> command_set const *;

            // opcode to uuid
            auto get_command_uuid(version ver, std::uint16_t opcode) const noexcept -> std::uint16_t
            {
                if (to_uint(ver) < std::size_t(m_cmd_to_uuid_map))
                    if (opcode < std::size(m_cmd_to_uuid_map[to_uint(ver)]))
                        return m_cmd_to_uuid_map[to_uint(ver)][opcode];
                return 0;
            }

            // opcode to command
            auto get_command(version ver, std::uint16_t opcode) const noexcept -> command const *
            {
                return get_command(get_command_uuid(ver, opcode));
            }

            // uuid to command
            auto get_command(std::uint16_t uuid) const noexcept -> command const *
            {
                if (uuid < std::size(m_uuid_to_cmd_map))
                    return m_uuid_to_cmd_map[uuid];
                return nullptr;
            }

        public:
            explicit command_manager(char const * root_path);

        protected:
            void reload(void);

        private:
            char                m_root_path[1024];
            command_set const * m_set_map[0x100];
            std::uint16_t       m_cmd_to_uuid_map[0x100][0x1000];   // version:opcode to uuid
            command const *     m_uuid_to_cmd_map[0x10000];         // uuid to command
            std::uint16_t       m_uuid_count;
    };
}
