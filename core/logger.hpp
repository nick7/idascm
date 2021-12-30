# pragma once
# include <cstdarg>
# include <cstdio>

# define IDASCM_LOG(L, F, ...) \
    if ((L) <= ::idascm::logger::instance().current_level()) \
    { \
        ::idascm::logger::instance().add_message(L, __FILE__, __LINE__, __FUNCTION__, F, __VA_ARGS__); \
    }

# define IDASCM_LOG_E(F, ...) IDASCM_LOG(::idascm::logger::level::error,   F, __VA_ARGS__)
# define IDASCM_LOG_W(F, ...) IDASCM_LOG(::idascm::logger::level::warning, F, __VA_ARGS__)
# define IDASCM_LOG_I(F, ...) IDASCM_LOG(::idascm::logger::level::info,    F, __VA_ARGS__)
# define IDASCM_LOG_D(F, ...) IDASCM_LOG(::idascm::logger::level::debug,   F, __VA_ARGS__)
# define IDASCM_LOG_T(F, ...) IDASCM_LOG(::idascm::logger::level::trace,   F, __VA_ARGS__)

namespace idascm
{
    class logger
    {
        public:
            enum class level : int
            {
                invalid,
                error,
                warning,
                info,
                debug,
                trace,
            };

            struct context
            {
                level           level;
                char const *    file;
                int             line;
                char const *    function;
            };

            class handler
            {
                public:
                    virtual void message(context const & ctx, char const * message) = 0;
            };

        public:
            static logger & instance(void);

        public:
            auto add_handler(handler * handler) -> bool;
            auto remove_handler(handler * handler) -> bool;
            void add_message(level level, char const * file, int line, char const * function, char const * format, ...);
            auto current_level(void) const -> level
            {
                return m_level;
            }

        protected:
            logger(void);
            ~logger(void);

        private:
            level       m_level;
            handler *   m_handlers[8];
    };

    auto to_string(logger::level level) noexcept -> char const *;
}
