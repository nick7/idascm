# include <engine/command.hpp>
# include <core/json.hpp>
# include <algorithm>
# include <cstdlib>

namespace idascm
{
    namespace
    {
        struct
        {
            argument_type   type;
            char const *    strings[4];
        }
        static const g_argument_type_table[] = \
        {
            { argument_type::unknown,       { "unknown"             }   },
            { argument_type::any,           { "any"                 }   },
            { argument_type::integer,       { "integer",    "int"   }   },
            { argument_type::real,          { "real"                }   },
            { argument_type::character,     { "character",  "char"  }   },
            { argument_type::global,        { "global"              }   },
            { argument_type::local,         { "local"               }   },
            { argument_type::variadic,      { "variadic",   "..."   }   },
            { argument_type::address,       { "address"             }   },
            { argument_type::int8,          { "int8"                }   },
            { argument_type::int16,         { "int16"               }   },
            { argument_type::int32,         { "int32"               }   },
            { argument_type::float32,       { "float32"             }   },
            { argument_type::string64,      { "string64"            }   },
        };
    }

    auto argument_type_from_string(char const * string) noexcept -> argument_type
    {
        if (string && string[0])
        {
            for (auto const & row : g_argument_type_table)
            {
                for (auto s : row.strings)
                    if (s && s[0] && 0 == std::strcmp(string, s))
                        return row.type;
            }
        }
        return argument_type::unknown;
    }

    auto argument_type_from_json(json_value const & value) noexcept -> argument_type
    {
        return argument_type_from_string(value.to_primitive().c_str());
    }

    auto operator == (command const & first, command const & second) noexcept -> bool
    {
        // if (first.opcode != second.opcode)
        //     return false;
        if (first.flags != second.flags)
            return false;
        if (first.argument_count != second.argument_count)
            return false;
        if (0 != std::strncmp(first.name, second.name, std::size(first.name) - 1))
            return false;
        for (std::size_t i = 0; i < std::min<std::size_t>(first.argument_count, std::size(first.argument_list)); ++ i)
            if (first.argument_list[i] != second.argument_list[i])
                return false;
        return true;
    }

    auto to_string(command_flag flag) noexcept -> char const *
    {
        switch (flag)
        {
            case command_flag_stop:
                return "stop";
            case command_flag_jump:
                return "jump";
            case command_flag_call:
                return "call";
            case command_flag_return:
                return "return";
            case command_flag_condition:
                return "condition";
            case command_flag_conditional:
                return "conditional";
            case command_flag_function_call:
                return "function_call";
            case command_flag_cleo:
                return "cleo";
        }
        return nullptr;
    }

    // auto opcode_from_json(json_value const & value) -> std::uint16_t
    // {
    //     if (value.c_str()[0] == '0' && value.c_str()[1] == 'x')
    //         return static_cast<std::uint16_t>(std::strtoul(value.c_str(), nullptr, 16));
    //     return static_cast<std::uint16_t>(std::strtoul(value.c_str(), nullptr, 10));
    // }

    auto command_from_json(json_object const & object) -> command
    {
        command command = {};
        
        // auto const opcode = value["opcode"];
        // if (! opcode.is_valid())
        //     return command;
        // command.opcode = opcode_from_json(opcode);
        
        auto const name = object["name"].to_primitive();
        if (name.is_valid())
        {
            std::strncpy(command.name, name.c_str(), std::size(command.name) - 1);
        }

        auto const flags = object["flags"].to_array();
        if (flags.is_valid())
        {
            for (std::size_t i = 0; i < flags.size(); ++ i)
            {
                for (std::uint8_t flag = 1; flag < 0x80; flag <<= 1)
                {
                    auto const name = to_string(command_flag(flag));
                    if (name && 0 == std::strcmp(flags.at(i).to_primitive().c_str(), name))
                        command.flags |= flag;
                }
            }
        }

        auto const arguments = object["arguments"];
        if (arguments.type() == json_type::primitive)
        {
            command.argument_count = std::atoi(arguments.to_primitive().c_str());
            for (std::size_t i = 0; i < command.argument_count; ++ i)
            {
                command.argument_list[i] = argument_type::any;
            }
        }
        else
        {
            auto const argument_array = arguments.to_array();
            command.argument_count = (std::uint8_t) argument_array.size();
            for (std::size_t i = 0; i < command.argument_count; ++ i)
            {
                auto const argument = argument_array[i];
                if (json_type::primitive == argument.type())
                {
                    command.argument_list[i] = argument_type_from_json(argument_array[i]);
                    continue;
                }
                command.argument_list[i] = argument_type_from_json(argument_array[i].to_object()["type"]);
            }
        }

        auto comment = object["comment"].to_primitive();
        if (comment.is_valid())
        {
            std::strncpy(command.comment, comment.c_str(), std::size(command.comment) - 1);
        }

        return command;
    }
}
