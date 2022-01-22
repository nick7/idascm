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
            type            type;
            char const *    strings[2];
        }
        static const g_type_table[] = \
        {
            { type::unknown,            { "unknown"                                         } },
            { type::any,                { "any"                                             } },
            { type::integer,            { "integer",            "int"                       } },
            { type::real,               { "real",               "float"                     } },
            { type::string,             { "string"                                          } },
            { type::variadic,           { "variadic",           "..."                       } },
            { type::constant,           { "constant",           "const"                     } },
            { type::variable,           { "variable",                                       } },
            { type::global,             { "global"                                          } },
            { type::local,              { "local"                                           } },
            { type::address,            { "address",            "label"                     } },
            { type::constant_integer,   { "constant_integer",   "const_int"                 } },
            { type::constant_real,      { "constant_real",      "const_float"               } },
            { type::constant_string,    { "constant_string",    "const_string"              } },
            { type::variable_integer,   { "variable_integer",   "variable_int"              } },
            { type::variable_real,      { "variable_real",      "variable_float"            } },
            { type::variable_string,    { "variable_string"                                 } },
            { type::global_integer,     { "global_integer",     "global_int"                } },
            { type::global_real,        { "global_real",        "global_float"              } },
            { type::global_string,      { "global_string"                                   } },
            { type::local_integer,      { "variable_integer",   "local_int"                 } },
            { type::local_real,         { "variable_real",      "local_float"               } },
            { type::local_string,       { "variable_string"                                 } },
        };
    }

    auto type_from_string(char const * string) noexcept -> type
    {
        if (string && string[0])
        {
            for (auto const & row : g_type_table)
            {
                for (auto s : row.strings)
                    if (s && s[0] && 0 == std::strcmp(string, s))
                        return row.type;
            }
        }
        return type::unknown;
    }

    auto type_from_json(json_primitive const & value) noexcept -> type
    {
        return type_from_string(value.c_str());
    }

    auto argument_from_json(json_value const & value) noexcept -> argument
    {
        argument arg = {};
        if (value.type() == json_type::primitive)
        {
            arg.type = type_from_json(value.to_primitive());
        }
        else
        {
            auto const object = value.to_object();
            auto const type = object["type"].to_primitive();
            if (type.is_valid())
                arg.type = type_from_json(type);
            auto const operand_type = object["operand_type"].to_primitive();
            if (operand_type.is_valid())
                arg.operand_type = operand_type_from_json(operand_type);
        }
        return arg;
    }

    auto operator == (argument const & first, argument const & second) noexcept -> bool
    {
        if (first.type != second.type)
            return false;
        if (first.operand_type != second.operand_type)
            return false;
        return true;
    }

    auto operator != (argument const & first, argument const & second) noexcept -> bool
    {
        return ! (first == second);
    }

    auto operator == (command const & first, command const & second) noexcept -> bool
    {
        if (first.flags != second.flags)
            return false;
        if (first.argument_count != second.argument_count)
            return false;
        if (first.name != second.name)
            return false;
        if (first.comment != second.comment)
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
            case command_flag_unsupported:
                return "unsupported";
            case command_flag_macro:
                return "macro";
            case command_flag_cleo:
                return "cleo";
        }
        return nullptr;
    }

    auto command_from_json(json_object const & object) -> command
    {
        command command = {};

        auto const name = object["name"].to_primitive();
        if (name.is_valid())
        {
            command.name = name.to_string();
        }

        auto const flags = object["flags"].to_array();
        if (flags.is_valid())
        {
            for (std::size_t i = 0; i < flags.size(); ++ i)
            {
                for (std::uint8_t b = 0; b < sizeof(command_flag) << 3; ++ b)
                {
                    auto const flag = command_flag(1 << b);
                    auto const name = to_string(flag);
                    if (name && 0 == std::strcmp(flags.at(i).to_primitive().c_str(), name))
                        command.flags |= flag;
                }
            }
        }

        auto const arguments = object["args"];
        if (arguments.type() == json_type::primitive)
        {
            command.argument_count = std::atoi(arguments.to_primitive().c_str());
            for (std::size_t i = 0; i < command.argument_count; ++ i)
            {
                command.argument_list[i] = { type::any };
            }
        }
        else
        {
            auto const argument_array = arguments.to_array();
            command.argument_count = (std::uint8_t) argument_array.size();
            for (std::size_t i = 0; i < command.argument_count; ++ i)
            {
                command.argument_list[i] = argument_from_json(argument_array[i]);
            }
        }

        auto comment = object["comment"].to_primitive();
        if (comment.is_valid())
        {
            command.comment = comment.to_string();
        }

        return command;
    }
}
