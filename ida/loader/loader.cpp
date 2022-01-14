# include <ida/loader/loader.hpp>
# include <ida/processor/processor.hpp>
# include <ida/base/memory_loader.hpp>
# include <engine/command_manager.hpp>
# include <engine/decoder/decoder_gta3.hpp>
# include <engine/decoder/decoder_gtavc.hpp>
# include <engine/loader.hpp>
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

            auto mem = memory_api_loader(li);
            auto dec = decoder_gta3();
            dec.set_memory_api(&mem);
            dec.set_command_set(base_command_manager().get_set(version::gta3_ps2));
            auto ldr = loader(&mem, &dec);
            if (ldr.load())
            {
                *fileformatname = version_description(version::gta3_ps2);
                return 1;
            }

            return 0;
        }

        void idaapi load_file(linput_t * li, ushort neflags, char const * fileformatname)
        {
            IDASCM_LOG_D("load_file: %p, %u, '%s'", li, neflags, fileformatname);
            auto mem = memory_api_loader(li);
            auto dec = decoder_gta3();
            dec.set_memory_api(&mem);
            dec.set_command_set(base_command_manager().get_set(version::gta3_ps2));
            auto ldr = loader(&mem, &dec);
            if (ldr.load())
            {
                set_processor_type(to_string(version::gta3_ps2), SETPROC_LOADER);
                file2base(li, 0, 0, qlsize(li), FILEREG_PATCHABLE);
                auto layout = ldr.get_layout();
                for (auto const & item : layout.segments)
                {
                    segment_t seg = {};
                    seg.sel      = 0;
                    seg.start_ea = item.address;
                    seg.end_ea   = item.address + item.size;
                    seg.align    = saRelByte;
                    seg.comb     = scPub;
                    seg.bitness  = 1; // 32-bit

                    char const * class_name = nullptr;
                    switch (item.type)
                    {
                        case segment_type::code:
                            seg.perm = SEGPERM_READ | SEGPERM_EXEC;
                            class_name = "CODE";
                            break;
                        case segment_type::data:
                            seg.perm = SEGPERM_READ | SEGPERM_WRITE;
                            class_name = "DATA";
                            break;
                        case segment_type::mixed:
                            seg.perm = SEGPERM_READ | SEGPERM_WRITE | SEGPERM_EXEC;
                            class_name = "CODE";
                            break;
                    }
                    add_segm_ex(&seg, item.name.c_str(), class_name, ADDSEG_NOSREG | ADDSEG_NOTRUNC | ADDSEG_QUIET);
                }
            }
        }

        // int idaapi save_file(FILE * fp, char const * fileformatname)
        // {
        // }

        auto setup_loader(void) noexcept -> loader_t
        {
            startup();
            loader_t loader         = {};
            loader.version          = IDP_INTERFACE_VERSION;
            loader.flags            = 0;
            loader.accept_file      = accept_file;
            loader.load_file        = load_file;
            // loader.save_file        = save_file;
            // loader.move_segm        = nullptr;
            // loader.process_archive  = nullptr;
            return loader;
        }
    }
}

loader_t LDSC = idascm::setup_loader();
