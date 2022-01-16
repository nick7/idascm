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
        auto load_json_object(std::string root, version ver) -> json_object
        {
            std::string const path = root + "/" + std::string(to_string(ver)) + ".json";
            IDASCM_LOG_I("Loading commands from '%s'", path.c_str());
            auto const json = json_value::from_file(path.c_str());
            if (json.is_valid())
            {
                auto const object = json.to_object();
                if (object.is_valid())
                {
                    return object;
                }
            }
            return {};
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
            m_set_map[index] = load_set(ver);
            reload();
        }
        return m_set_map[index];
    }

    auto command_manager::load_set(version ver) -> command_set *
    {
        auto object = load_json_object(m_root_path, ver);
        if (! object.is_valid())
            return nullptr;
        if (to_version(object["version"].to_primitive().c_str()) != ver)
            return false;
        command_set const * parent_set = nullptr;
        auto parent = object["parent"].to_primitive();
        if (parent.is_valid())
        {
            auto const parent_version = to_version(parent.c_str());
            if (parent_version == version::unknown)
            {
                IDASCM_LOG_W("invalid parent version: '%s'", parent.c_str());
                return nullptr;
            }
            parent_set = get_set(parent_version);
            if (! parent_set)
            {
                IDASCM_LOG_W("unable to load parent command set: '%s'", to_string(parent_version));
                return nullptr;
            }
        }
        auto set = new command_set(ver);
        if (nullptr == parent_set || set->set_parent(parent_set))
        {
            auto commands = object["commands"].to_object();
            if (commands.is_valid())
            {
                if (set->load(commands))
                {
                    return set;
                }
            }
            else
            {
                IDASCM_LOG_W("invalid 'commands' object");
                return set;
            }
        }
        delete set;
        return nullptr;
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
