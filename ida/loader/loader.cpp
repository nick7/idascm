# include <ida/loader/loader.hpp>
# include <ida/processor/processor.hpp>
# include <ida/utils/utils.hpp>
# include <engine/decoder/decoder.hpp>
# include <core/logger.hpp>
# include <diskio.hpp>
# include <cassert>

namespace idascm
{
    // 02 00 01 - GTA3 / GTAVC / GTASA
    //

    namespace
    {
        char const gs_gta3_scm[]    = "GTA III Compiled Script File";
        char const gs_gtalcs_scm[]  = "GTA LCS/VCS Compiled Script File";


        void startup(void)
        {
            initialize_ida_logger();
            IDASCM_LOG_I("idascm_ldr %s (IDA_SDK_VERSION=%d)", build_version(), IDA_SDK_VERSION);
        }

        int idaapi accept_file(qstring * fileformatname, qstring * processor, linput_t * li, const char * filename)
        {
            IDASCM_LOG_D("accept_file: '%s', '%s', %p, '%s'", fileformatname, processor->c_str(), li, filename);
            qlseek(li, 0, SEEK_SET);

            std::uint8_t header[16];
            if (qlread(li, header, sizeof(header)) != sizeof(header))
                return 0;

            if (header[0] == 0x02 && header[1] == 0x00) // GOTO
            {
                if (header[2] == 0x01) // int32
                {
                    assert(fileformatname);
                    *fileformatname = gs_gta3_scm;
                    return 1;
                }
            }
            if (header[8 + 0] == 0x02 && header[8 + 1] == 0x00) // GOTO
            {
                if (header[8 + 2] == 0x06) // int32
                {
                    assert(fileformatname);
                    *fileformatname = gs_gtalcs_scm;
                    return 1;
                }
            }

            return 0;
        }

        void idaapi load_file(linput_t * li, ushort neflags, char const * fileformatname)
        {
            IDASCM_LOG_D("load_file: %p, %u, '%s'", li, neflags, fileformatname);
            if (processor_id != ph.id)
            {
                // set_processor_type("idascm", SETPROC_LOADER);
                // set_target_assembler(0);
            }
        }

        // int idaapi save_file(FILE * fp, char const * fileformatname)
        // {
        // }
    }

    auto loader(void) noexcept -> loader_t
    {
        startup();
        loader_t loader         = {};
        loader.version          = IDP_INTERFACE_VERSION;
        loader.flags            = 0;
        loader.accept_file      = accept_file;
        loader.load_file        = load_file;
        // loader.save_file        = save_file;
        loader.move_segm        = nullptr;
        loader.process_archive  = nullptr;
        return loader;
    }
}

loader_t LDSC = idascm::loader();
