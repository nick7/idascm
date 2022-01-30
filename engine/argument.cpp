# include <engine/argument.hpp>
# include <core/json.hpp>

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
            { type::mission,            { "mission"                                         } },
            { type::script,             { "script"                                          } },
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
}
