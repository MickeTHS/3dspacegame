#include "engine/data_archive.h"

#include "engine/gfx_object.h"
#include "engine/gfx_shader.h"

bool Data_toc_entry::sort(Data_toc_entry a, Data_toc_entry b) {
    int at = Data_archive::type_to_order(Data_archive::name_to_type(a.filename));
    int bt = Data_archive::type_to_order(Data_archive::name_to_type(b.filename));

    if (at == bt) {
        return std::strcmp(a.filename, b.filename) < 0;
    }

    return at < bt;
}

Data_archive::Data_archive() {}

bool Data_archive::add_subfile(std::shared_ptr<Data_disk_file> file) {
    _files.push_back(file);

    return true;
}

bool Data_archive::add_file(const char* path, const char* data_filename, Data_toc_entry& toc_entry) {
    auto exists = get_file(toc_entry.filename);
    if (exists != nullptr) {
        return true;
    }

    std::shared_ptr<Data_disk_file> sf = std::make_shared<Data_disk_file>();
    strcpy(sf->header.filename, toc_entry.filename);
    sf->header.filesize = toc_entry.filesize;
    sf->header.filetype = name_to_type(toc_entry.filename);
    
    _files.push_back(sf);

    return true;
}



bool Data_archive::add_file(const char* path, const char* filename) {
    auto exists = get_file(filename);
    if (exists != nullptr) {
        return true;
    }

    std::string fullpath;

    if (path[0] != 0) {
        fullpath = std::string(path) + "/" + std::string(filename);
    }
    else {
        fullpath = std::string(filename);
    }

    if (!Data_utils::file_exists(fullpath)) {
        return false;
    }

    std::shared_ptr<Data_disk_file> sf = std::make_shared<Data_disk_file>();
    strcpy(sf->header.filename, filename);
    sf->header.filesize = static_cast<uint32_t>(Data_utils::filesize(fullpath.c_str()));
    sf->header.filetype = name_to_type(filename);
    
    _files.push_back(sf);

    return true;
}

void Data_archive::generate_toc(std::vector<Data_toc_entry>& toc) {
    toc.clear();
    for (auto&& sf : _files) {
        Data_toc_entry t;
        strcpy(t.filename, sf->header.filename);
        t.filesize = sf->header.filesize;
        toc.push_back(t);
    }

    calculate_toc_offsets(toc);
}

std::shared_ptr<Data_disk_file> Data_archive::get_file(const char* filename) {
    for (auto&& f : _files) {
        if (strcmp(f->header.filename, filename) == 0) {
            return f;
        }
    }

    return nullptr;
}

uint32_t Data_archive::calc_filesize(std::vector<Data_toc_entry>& toc) {
    //uint32_t filesize = sizeof(uint32_t) + DATAFILE_FILENAME_MAX_LENGTH + sizeof(uint32_t); // filesize, filename, filetype (ARCHIVE)
    uint32_t filesize = sizeof(Data_disk_file_header);

    for (auto&& t : toc) {
        filesize += t.filesize;
    }

    return filesize;
}

void Data_archive::write(const char* filename) {
    std::vector<Data_toc_entry> toc;
    generate_toc(toc);

    FILE* fp = fopen(filename, "wb");
    
    std::string tmp(filename);
    tmp = tmp.substr(tmp.find_last_of("\\/")+1);

    Data_disk_file_header header;

    header.filesize = calc_filesize(toc);;
    header.filetype = Data_filetype::ARCHIVE;
    memset(header.filename, 0, DATAFILE_FILENAME_MAX_LENGTH);
    strcpy(header.filename, tmp.c_str());

    fwrite(&header, sizeof(Data_disk_file_header), 1, fp);

    write_toc(toc, fp);
    
    for (auto&& f : _files) {
        f->write(fp);
    }

    fclose(fp);
}

int Data_archive::type_to_order(Data_filetype type) {
    switch(type) {
        case Data_filetype::DDS: return 0;
        case Data_filetype::MODEL: return 1;
        case Data_filetype::SKELETON: return 2;
        case Data_filetype::ANIMATION: return 3;
        case Data_filetype::LIGHTS: return 4;
        case Data_filetype::AREA: return 5;
        case Data_filetype::SOUND: return 6;
        default: return 9;
    }
}

void Data_archive::optimize_toc(std::vector<Data_toc_entry>& toc) {
    std::vector<Data_toc_entry> copy;

    for (auto&& t : toc) {
        bool found = false;
        for (auto&& t2 : copy) {
            if (strcmp(t2.filename, t.filename) == 0) {
                found = true;
                break;
            }
        }

        if (!found) {
            copy.push_back(t);
        }
    }

    std::sort(copy.begin(), copy.end(), Data_toc_entry::sort);

    toc.clear();

    for (auto&& t : copy) {
        toc.push_back(t);
    }
}

uint32_t Data_archive::calc_toc_size(std::vector<Data_toc_entry>& toc) {
    return sizeof(uint32_t) + sizeof(Data_toc_entry) * toc.size();
}

void Data_archive::calculate_toc_offsets(std::vector<Data_toc_entry>& toc) {
    optimize_toc(toc);

    uint32_t offset = sizeof(uint32_t) + DATAFILE_FILENAME_MAX_LENGTH + sizeof(uint32_t);
    
    offset += calc_toc_size(toc);

    for (auto&& t : toc) {
        t.offset = offset;
        offset += t.filesize;
    }
}

Data_filetype Data_archive::name_to_type(const char* filename) {
    std::string fn = std::string(filename);
    std::string tn = fn.substr(fn.find_last_of(".")+1);

    if (tn == "area") {
        return Data_filetype::AREA;
    }
    if (tn == "lights") {
        return Data_filetype::LIGHTS;
    }
    if (tn == "mdl") {
        return Data_filetype::MODEL;
    }
    if (tn == "dds") {
        return Data_filetype::DDS;
    }
    if (tn == "anim") {
        return Data_filetype::ANIMATION;
    }
    if (tn == "skel") {
        return Data_filetype::SKELETON;
    }
    if (tn == "otsound" || tn == "wav") {
        return Data_filetype::SOUND;
    }
    if (tn == "vd") {
        return Data_filetype::VEHICLE_DESCRIPTION;
    }
    if (tn == "arc") {
        return Data_filetype::ARCHIVE;
    }
    return Data_filetype::NONE;
}

bool Data_archive::read_file_in_archive(const char* pkgfilename, const char* filename, std::shared_ptr<Data_disk_file>& data_file) {
    if (data_file == nullptr) {
        printf("ERROR: invalid access to read_file_in_pkg, Data_file must be a valid shared_ptr\n");
        return false;
    }
    /*
    std::vector<Data_toc_entry> toc;

    read_toc(pkgfilename, toc);

    for (auto t : toc) {
        if (strcmp(filename, t.filename) == 0) {
            data_file->filetype = name_to_type(t.filename);
            data_file->data.resize(t.filesize);
            
            FILE *infile = fopen(pkgfilename, "rb");
            fseek(infile, t.offset, SEEK_SET);
            fread(&Data_file->data[0], t.filesize, 1, infile);
            fclose(infile);
        
            return true;
        }
    }*/

    return false;
}

Data_archive::~Data_archive() {
    while(!_textures.empty()) {
        _textures.erase(_textures.begin());
    }

    while (!_models.empty()) {
        _models.erase(_models.begin());
    }

    for (auto f : _files) {
        if (f->data != NULL) {
            int apa = 0;
            free(f->data);
        }
    }
}

bool Data_archive::open(const char* path) {
    _fp = fopen(path, "rb");

    Data_disk_file_header dfh;

    fread(&dfh, sizeof(Data_disk_file_header), 1, _fp);

    if (dfh.filetype != Data_filetype::ARCHIVE) {
        fclose(_fp);
        return false;
    }

    read_toc(_fp);

    return true;
}

// working texture loader
std::shared_ptr<Gfx_texture> Data_archive::load_texture_file(const char* name) {
    auto ff = get_file(name);

    if (ff != nullptr) {
        for (auto t : _textures) {
            if (strcmp(t->name, name) == 0) {
                return t;
            }
        }
    }

    for (auto t : _toc) {
        if (strcmp(t.filename, name) == 0) {
            
            std::shared_ptr<Data_disk_file> df = std::make_shared<Data_disk_file>();
            
            fseek(_fp, t.offset, SEEK_SET);
            fread(df.get(), sizeof(Data_disk_file_header), 1, _fp);

            uint32_t disk_file_data_size = df->header.filesize - sizeof(Data_disk_file_header);
            df->data = malloc(disk_file_data_size);
            fread(df->data, disk_file_data_size, 1, _fp);

            _files.push_back(df);

            Data_texture dtt;

            memcpy(&dtt.header.hash, &((uint8_t*)df->data)[0], sizeof(uint32_t));
            memcpy(&dtt.header.datasize, &((uint8_t*)df->data)[4], sizeof(uint32_t));
            dtt.data = &((uint8_t*)df->data)[8];
            
            std::shared_ptr<Gfx_texture> gfx_tex = std::make_shared<Gfx_texture>();
            
            strcpy(gfx_tex->name, df->header.filename);
            gfx_tex->textureId = gfx_tex->load_dds_buffer(dtt.data);

            assert(gfx_tex->textureId >= 0);

            return gfx_tex;
        }
    }

    return nullptr;
}

std::shared_ptr<Gfx_object> Data_archive::load_model_file(const char* name) {
    auto ff = get_file(name);

    /*
    if (ff != nullptr) {
        for (auto t : _models) {
            if (strcmp(t->header.filename.c_str(), name) == 0) {
                return t;
            }
        }
    }*/

    for (auto t : _toc) {
        if (strcmp(t.filename, name) == 0) {

            std::shared_ptr<Data_disk_file> df = std::make_shared<Data_disk_file>();

            fseek(_fp, t.offset, SEEK_SET);
            fread(df.get(), sizeof(Data_disk_file_header), 1, _fp);

            uint32_t disk_file_data_size = df->header.filesize - sizeof(Data_disk_file_header);
            df->data = malloc(disk_file_data_size);
            fread(df->data, disk_file_data_size, 1, _fp);

            _files.push_back(df);

            Data_model dtm;

            memcpy(&dtm.header, &((uint8_t*)df->data)[0], sizeof(Data_model_header));

            uint32_t offset = sizeof(Data_model_header);

            dtm.verticesdata = &((uint8_t*)df->data)[offset];
            offset += dtm.header.vertices_data_size;

            dtm.indicedata = &((uint8_t*)df->data)[offset];
            
            std::shared_ptr<SEVBO> sevbo = std::make_shared<SEVBO>();
            sevbo->indice_buffer_size = dtm.header.indices_data_size;
            sevbo->vertex_buffer_size = dtm.header.vertices_data_size;
            sevbo->indice_data = dtm.indicedata;
            sevbo->vert_data = dtm.verticesdata;
            sevbo->num_indices = dtm.header.indices_data_size / sizeof(uint32_t);
            sevbo->num_vertices = dtm.header.vertices_data_size / SEVBO::typesize(dtm.header.rendertype);

            std::shared_ptr<Gfx_draw_object> drawobj = std::make_shared<Gfx_draw_object>();
            drawobj->setup(sevbo, Gfx_shader_store::get_instance().get_shader("posnormuv"));
            
            std::shared_ptr<Gfx_object> obj = std::make_shared<Gfx_object>(sevbo, drawobj);
            memcpy(&obj->local_bounds[0], &dtm.header.bb[0], sizeof(float)*24);
            obj->set_pos(glm::vec3(0,0,0));
            obj->update();

            _models.push_back(obj);

            return obj;
        }
    }

    return nullptr;
}



bool Data_archive::read_toc(FILE* fp) {

    uint32_t num_files = 0;

    fread(&num_files, sizeof(uint32_t), 1, fp);

    _toc.resize(num_files);
    fread(&_toc[0], num_files * sizeof(Data_toc_entry), 1, fp);
    
    return true;
}

bool Data_archive::read_toc(const std::string& filename, std::vector<Data_toc_entry>& toc) {

    uint32_t num_files = 0;

    FILE *infile = fopen(filename.c_str(), "rb");

    if (infile == NULL) {
        printf("ERROR: unable to find file: %s\n", filename.c_str());
        return false;
    }

    fread(&num_files, sizeof(uint32_t), 1, infile);

    toc.resize(num_files);
    fread(&toc[0], num_files * sizeof(Data_toc_entry), 1, infile);
    fclose(infile);

    return true;
}

bool Data_archive::write_toc(const std::vector<Data_toc_entry>& toc, FILE* outfile) {
    uint32_t num_files = toc.size();

    fwrite(&num_files, sizeof(uint32_t), 1, outfile);
    fwrite(&toc[0], sizeof(Data_toc_entry) * num_files, 1, outfile);

    return true;
}
