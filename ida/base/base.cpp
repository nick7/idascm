# include <ida/base/base.hpp>
# include <engine/command_manager.hpp>
# include <core/logger.hpp>
# include <windows.h>

namespace idascm
{
    namespace
    {
        class ida_logger : public logger::handler
        {
            public:
                virtual void message(logger::context const & ctx, char const * message) override
                {
                    format("[%s][%s] %s", "idascm", to_string(ctx.level), message);
                }

                static void format(char const * format, ...)
                {
                    va_list args;
                    va_start(args, format);
                    callui(ui_msg, format, args);
                    va_end(args);
                }

                ida_logger(void)
                    : handler()
                {
                    logger::instance().add_handler(this);
                }

                ~ida_logger(void)
                {
                    logger::instance().remove_handler(this);
                }
        };
    }

    void initialize_ida_logger(void)
    {
        // EA64 version crashes at callui(ui_msg, ...) O_o
# ifndef __EA64__
        static ida_logger logger;
# endif
    }

    namespace
    {
        auto data_root_path(void) -> char const *
        {
            static char root[1024];

            CHAR path[1024] = {};
            GetModuleFileNameA(GetModuleHandleA(NULL), path, sizeof(path) - 1);

            CHAR * file = nullptr;
            GetFullPathNameA(path, sizeof(root), root, &file);
            if (file)
            {
                *file = 0;
            }
            qstrncat(root, "cfg\\idascm\\", sizeof(root) - 1);
            return root;
        }
    }

    auto base_command_manager(void) -> command_manager &
    {
        static command_manager manager(data_root_path());
        return manager;
    }
}
