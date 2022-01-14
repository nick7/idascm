# pragma once
# if 1 // sdk shit fix
#   define print_predefined_segname static print_predefined_segname
#   include <idaidp.hpp>
#   undef print_predefined_segname
# else
#   include <idaidp.hpp>
# endif

namespace idascm
{
    class command_manager;

    void initialize_ida_logger(void);

    auto base_command_manager(void) -> command_manager &;
}
