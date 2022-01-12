# pragma once
# if 1 // sdk shit fix
#   define print_predefined_segname static print_predefined_segname
#   include <idaidp.hpp>
#   undef print_predefined_segname
# else
#   include <idaidp.hpp>
# endif
# if IDA_SDK_VERSION < 750
#   define IDASCM_STATIC_MODULE_INSTANCE
# endif

namespace idascm
{
    void initialize_ida_logger(void);
}
