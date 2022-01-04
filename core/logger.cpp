# include <core/logger.hpp>
# include <algorithm>
# include <cstdarg>
# include <cstdio>
# if defined IDASCM_PLATFORM_WINDOWS
#   include <windows.h>
# endif

namespace idascm
{
    namespace
    {
        char const * const g_level_table[] = \
        {
            "invalid",
            "error",
            "warning",
            "info",
            "debug",
            "trace",
        };

        char const * const g_prefix_table[] = \
        {
            nullptr,
            "err",
            "war",
            "inf",
            "deb",
            "tra",
        };

        auto level_to_prefix(logger::level level) -> char const *
        {
            if (std::size_t(level) < std::size_t(g_prefix_table))
                return g_prefix_table[std::size_t(level)];
            return nullptr;
        }

# if defined IDASCM_PLATFORM_WINDOWS
        class win32_debug_handler : public logger::handler
        {
            public:
                virtual void message(logger::context const & ctx, char const * message) override
                {
                    WCHAR buffer[2048];
                    ::wsprintfW(buffer, L"%hs(%d): [%hs] %hs", ctx.file, ctx.line, level_to_prefix(ctx.level), message);
                    OutputDebugStringW(buffer);
                }
        };
# endif

        class file_handler : public logger::handler
        {
            public:
                virtual void message(logger::context const & ctx, char const * message) override
                {
                    if (! m_file)
                        return;
                    std::fprintf(m_file, "%s(%d): [%s] %s", ctx.file, ctx.line, level_to_prefix(ctx.level), message);
                }

                file_handler(void)
                    : m_file(nullptr)
                {
                    m_file = std::fopen("idascm.log", "wb");
                }

                ~file_handler(void)
                {
                    if (m_file)
                    {
                        std::fflush(m_file);
                        std::fclose(m_file);
                        m_file = nullptr;
                    }
                }

            private:
                std::FILE * m_file;
        };
    }

    logger & logger::instance(void)
    {
        static logger instance;
        return instance;
    }

    logger::logger(void)
        : m_level(level::info)
    {
        std::memset(m_handlers, 0, sizeof(m_handlers));
# if defined IDASCM_BUILD_DEBUG
        static file_handler file;
        add_handler(&file);
        set_level(level::debug);
# endif
# if defined IDASCM_PLATFORM_WINDOWS
        static win32_debug_handler debug;
        add_handler(&debug);
# endif
    }

    logger::~logger(void)
    {
    }

    auto logger::add_handler(handler * handler) -> bool
    {
        for (std::size_t i = 0; i < std::size(m_handlers); ++ i)
        {
            if (m_handlers[i])
                continue;
            m_handlers[i] = handler;
            return true;
        }
        return false;
    }

    auto logger::remove_handler(handler * handler) -> bool
    {
        for (std::size_t i = 0; i < std::size(m_handlers); ++ i)
        {
            if (m_handlers[i] != handler)
                continue;
            m_handlers[i] = nullptr;
            return true;
        }
        return false;
    }

    void logger::add_message(level level, char const * file, int line, char const * function, char const * format, ...)
    {
        if (level > m_level)
            return;
        char message[1024] = { 0 };
        va_list args;
        va_start(args, format);
        int length = std::vsnprintf(message, sizeof(message) - 1, format, args);
        va_end(args);
        if (length && message[length - 1] != '\n')
        {
            message[length++] = '\n';
            message[length++] = '\0';
        }
        
        context const ctx = \
        {
            level,
            file,
            line,
            function
        };
        for (auto const & handler : m_handlers)
        {
            if (handler)
                handler->message(ctx, message);
        }
    }

    auto to_string(logger::level level) noexcept -> char const *
    {
        if (std::size_t(level) < std::size(g_level_table))
            return g_level_table[std::size_t(level)];
        return nullptr;
    }
}
