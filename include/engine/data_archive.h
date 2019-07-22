#pragma once

#include "pch.h"
#include "engine/data_file.h"
#include "engine/gfx_texture.h"
#include "engine/gfx_sevbo.h"
#include "engine/gfx_object.h"
#include "engine/gfx_skinning.h"
#include "engine/gfx_animation.h"

struct Data_toc_entry {
    char filename[DATAFILE_FILENAME_MAX_LENGTH];
    uint32_t filesize;
    uint32_t offset;

    static bool sort(Data_toc_entry a, Data_toc_entry b);
};


struct Data_archive {
public:
    Data_archive();
    virtual ~Data_archive();
    // production functions
    bool open(const char* path);
    std::shared_ptr<Gfx_texture> load_texture_file(const char* name);
    std::shared_ptr<Gfx_object> load_model_file(const char* name);
    std::shared_ptr<Gfx_skeleton> load_skeleton_file(const char* name);
    std::shared_ptr<Gfx_animation> load_animation_file(const char* name);

    // offline functions
    bool read_file_and_add(const char* path);
    bool add_file(const char* path, const char* data_filename, Data_toc_entry& toc_entry);
    bool add_file(const char* path, const char* filename);
    bool add_subfile(std::shared_ptr<Data_disk_file> file);
    void generate_toc(std::vector<Data_toc_entry>& toc);
    uint32_t calc_filesize(std::vector<Data_toc_entry>& toc);

    void write(const char* filename);

    std::shared_ptr<Data_disk_file> get_file(const char* filename);

    static void optimize_toc(std::vector<Data_toc_entry>& toc);
    static void calculate_toc_offsets(std::vector<Data_toc_entry>& toc);
    static uint32_t calc_toc_size(std::vector<Data_toc_entry>& toc);
    static bool read_toc(const std::string& filename, std::vector<Data_toc_entry>& toc);
    static bool write_toc(const std::vector<Data_toc_entry>& toc, FILE* outfile);
    static bool read_file_in_archive(const char* archive_filename, const char* filename, std::shared_ptr<Data_disk_file>& data_file);

    static Data_filetype name_to_type(const char* filename);
    static int type_to_order(Data_filetype type);

private:
    // production
    FILE* _fp;
    std::vector<Data_toc_entry> _toc;
    bool read_toc(FILE* fp);
    std::vector<std::shared_ptr<Data_disk_file>> _files;
    std::vector<std::shared_ptr<Gfx_texture>> _textures;
    // std::vector<std::shared_ptr<SEVBO>> _models; // maybe SEVBO??
    std::vector<std::shared_ptr<Gfx_object>> _models; // maybe SEVBO??

};