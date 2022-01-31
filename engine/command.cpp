# include <engine/command.hpp>
# include <core/json.hpp>
# include <core/logger.hpp>

namespace idascm
{
    auto operator == (command const & first, command const & second) noexcept -> bool
    {
        if (first.flags != second.flags)
            return false;
        if (first.arguments.size() != second.arguments.size())
            return false;
        if (first.name != second.name)
            return false;
        if (first.description != second.description)
            return false;
        if (first.comment != second.comment)
            return false;
        if (first.arguments != second.arguments)
            return false;
        return true;
    }

    auto to_string(command_flag flag) noexcept -> char const *
    {
        switch (flag)
        {
            case command_flag_stop:
                return "stop";
            case command_flag_branch:
                return "branch";
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
            case command_flag_switch:
                return "switch";
            case command_flag_dependent:
                return "dependent";
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
            argument any = {};
            any.type = type::any;
            command.arguments.resize(std::atoi(arguments.to_primitive().c_str()), any);
        }
        else
        {
            auto const argument_array = arguments.to_array();
            auto const argument_count = argument_array.size();
            command.arguments.reserve(argument_count);
            for (std::size_t i = 0; i < argument_count; ++ i)
            {
                command.arguments.push_back(argument_from_json(argument_array[i]));
                if (command.arguments.back().type == type::variadic)
                {
                    if (i != argument_count - 1)
                    {
                        IDASCM_LOG_W("only last argument can be variadic");
                    }
                }
            }
        }

        auto comment = object["comment"].to_primitive();
        if (comment.is_valid())
        {
            command.comment = comment.to_string();
        }

        auto description = object["description"].to_primitive();
        if (description.is_valid())
        {
            command.description = description.to_string();
        }

        return command;
    }
}
