# pragma once
# include <processor/processor.hpp>
# include <engine/decoder.hpp>

namespace idascm
{
    class command_set;
    class decoder;

    class analyzer;
    class emulator;
    class output;

    class module
# if IDA_SDK_VERSION >= 750
        : public procmod_t
# endif
    {
        public:
            virtual ssize_t idaapi on_event(ssize_t msgid, va_list va);

            void set_data_id(int data_id)
            {
                m_data_id = data_id;
            }

        public:
            module(void);
            ~module(void);

        private:
            int                 m_proc;
            int                 m_data_id;
            command_set const * m_isa;
            analyzer *          m_analyzer; // 'ana'
            emulator *          m_emulator; // 'emu'
            output *            m_output;   // 'out'
    };
}
