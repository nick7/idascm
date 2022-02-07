# pragma once
# include <ida/processor/processor.hpp>
# include <ida/base/memory_ida.hpp>

namespace idascm
{
    class command_set;
    class decoder;

    class analyzer;
    class emulator;
    class memory_api_ida;
    class output;

    class module
# if IDA_SDK_VERSION >= 750
        : public procmod_t
# endif
    {
        public:
            virtual auto idaapi on_event(ssize_t msgid, va_list va) -> ssize_t;

            auto idaapi set_idp_options(char const * keyword, int value_type, void const * value) -> const char *;

            void set_data_id(int data_id)
            {
                m_data_id = data_id;
            }

            void set_flag(processor_flag flag, bool enable)
            {
                m_flags = enable ? (m_flags | to_uint(flag)) : (m_flags & ~to_uint(flag));
            }

            auto set_version(version ver) -> bool;

        public:
            module(void);
            ~module(void);

        private:
            int                 m_data_id;
            std::uint32_t       m_flags;
            memory_api_ida      m_memory;
            command_set const * m_isa;
            analyzer *          m_analyzer; // 'ana'
            emulator *          m_emulator; // 'emu'
            output *            m_output;   // 'out'
    };
}
