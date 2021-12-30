# include <engine/command_manager.hpp>
# include <engine/command_set.hpp>
# include <core/json.hpp>
# include <core/logger.hpp>
# include <algorithm>
# include <string>
# if defined _WIN32
#   include <windows.h>
# endif

namespace idascm
{
    namespace
    {
        auto load_command_set(std::string root, version ver) -> command_set *
        {
            std::string const path = root + "/" + std::string(to_string(ver)) + ".json";
            IDASCM_LOG_I("Loading commands from '%s'", path.c_str());
            auto set = new command_set;
            if (set->load(json_value::from_file(path.c_str())))
            {
                return set;
            }
            delete set;
            return nullptr;
        }
    }

    command_manager::command_manager(char const * root_path)
        : m_uuid_count(0)
    {
        std::strncpy(m_root_path, root_path ? root_path : "", sizeof(m_root_path) - 1);
        std::memset(m_set_map, 0, sizeof(m_set_map));
        std::memset(m_uuid_to_cmd_map, 0x00, sizeof(m_uuid_to_cmd_map));
        std::memset(m_cmd_to_uuid_map, 0x00, sizeof(m_cmd_to_uuid_map));
    }

    auto command_manager::get_set(version ver) noexcept -> command_set const *
    {
        auto const index = to_uint(ver);
        if (index >= std::size(m_set_map))
            return nullptr;
        if (! m_set_map[index])
        {
            m_set_map[index] = load_command_set(m_root_path, ver);
            reload();
        }
        return m_set_map[index];
    }

    void command_manager::reload(void)
    {
        std::memset(m_uuid_to_cmd_map, 0x00, sizeof(m_uuid_to_cmd_map));
        std::memset(m_cmd_to_uuid_map, 0x00, sizeof(m_cmd_to_uuid_map));
        m_uuid_count = 1; // uuid 0 is reserved as invalid value
        for (std::size_t ver = 0; ver < std::size(m_set_map); ++ ver)
        {
            if (! m_set_map[ver])
                continue;
            for (std::uint16_t op = 0; op < 0x1000; ++ op)
            {
                if (auto cmd = m_set_map[ver]->get_command(op))
                {
                    command const * copy = nullptr;
                    for (std::size_t i = 0; i < ver; ++ i)
                    {
                        if (! m_set_map[i])
                            continue;
                        if (auto other = m_uuid_to_cmd_map[m_cmd_to_uuid_map[i][op]])
                        {
                            if (*other == *cmd)
                            {
                                m_cmd_to_uuid_map[ver][op] = m_cmd_to_uuid_map[i][op];
                                copy = other;
                                break;
                            }
                        }
                    }
                    if (! copy)
                    {
                        m_cmd_to_uuid_map[ver][op] = m_uuid_count;
                        m_uuid_to_cmd_map[m_uuid_count++] = cmd;
                    }
                }
            }
        }
    }
}
