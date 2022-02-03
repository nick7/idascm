# include <ida/loader/loader.hpp>
# include <ida/processor/processor.hpp>
# include <ida/base/memory_loader.hpp>
# include <engine/command_manager.hpp>
# include <engine/gtasa/decoder_gtasa.hpp>
# include <engine/gtasa/loader_gtasa.hpp>
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

        auto idaapi accept_file(qstring * fileformatname, qstring * processor, linput_t * li, const char * filename) -> int
        {
            IDASCM_LOG_D("accept_file: '%s', '%s', %p, '%s'", fileformatname, processor->c_str(), li, filename);
            int result = 0;
            auto mem = memory_api_loader(li);
            auto const ver = version::gtasa;
            auto const isa = base_command_manager().get_set(ver);
            if (isa)
            {
                auto dec = decoder::create(version_game(ver), *isa, mem);
                if (dec)
                {
                    auto ldr = loader::create(version_game(ver), *dec, mem);
                    if (ldr && ldr->load())
                    {
                        *fileformatname = version_description(ver);
                        result = 1;
                    }
                    delete ldr;
                    delete dec;
                }
            }
            return result;
        }

        void idaapi load_file(linput_t * li, ushort neflags, char const * fileformatname)
        {
            IDASCM_LOG_D("load_file: %p, %u, '%s'", li, neflags, fileformatname);
            auto mem = memory_api_loader(li);
            auto const ver = version::gtasa;
            auto const isa = base_command_manager().get_set(ver);
            if (! isa)
            {
                IDASCM_LOG_W("unable to get command set '%s'", to_string(ver));
                return;
            }
            if (auto dec = decoder::create(version_game(ver), *isa, mem))
            {
                auto ldr = loader::create(version_game(ver), *dec, mem);
                if (ldr && ldr->load())
                {
                    // set_processor_type(to_string(version::gtasa_ps2), SETPROC_LOADER);
                    file2base(li, 0, 0, qlsize(li), FILEREG_PATCHABLE);
                    auto layout = ldr->get_layout();
                    for (auto const & item : layout.segments)
                    {
                        segment_t seg = {};
                        seg.sel      = 0;
                        seg.start_ea = item.address;
                        seg.end_ea   = item.address + std::min(item.size, static_cast<std::uint32_t>(qlsize(li) - item.address));
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
                            case segment_type::globals:
                                seg.perm = SEGPERM_READ | SEGPERM_WRITE;
                                class_name = "DATA";
                                break;
                            case segment_type::readonly:
                                seg.perm = SEGPERM_READ;
                                class_name = "DATA";
                                break;
                            case segment_type::mixed:
                                seg.perm = SEGPERM_READ | SEGPERM_WRITE | SEGPERM_EXEC;
                                class_name = "CODE";
                                break;
                        }

                        if (! add_segm_ex(&seg, item.name.c_str(), class_name, ADDSEG_NOSREG | ADDSEG_NOTRUNC | ADDSEG_QUIET))
                        {
                            IDASCM_LOG_W("add_segm_ex failed!");
                        }
                    }
                }
                delete ldr;
                delete dec;
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
