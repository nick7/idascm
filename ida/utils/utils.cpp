# include <ida/utils/utils.hpp>
# include <core/logger.hpp>

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
}
