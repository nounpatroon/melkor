/*
Copyright (C) 2023  Juan Carlos Peña Gonzalez

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

DESCRIPTION:
    WAD in C++
*/

#ifndef __w_wad_hh__
#define __w_wad_hh__

#include <vector>
#include <fstream>
#include <string>
#include <cstdint>

extern "C" {

class wad_c {
public:
    wad_c();
    ~wad_c();

    void open(const char *path);

    uint32_t get_numlumps();
    const char* get_lump_data(const uint32_t index);
    const char* get_directory_name(const uint32_t index);
    uint32_t get_directory_size(const uint32_t index);
    const char* get_path();
    const char* get_identification();

    void set_identification(const char *mode);
    

    void insert(const uint32_t index, const char* name, char *data, const size_t size);
    void erase(const uint32_t index);

private:
    typedef struct {
        char identification[4];
        uint32_t numlumps;
        uint32_t infotableofs;
    } wadinfo_t;

    typedef struct {
        uint32_t filepos;
        uint32_t size;
        char name[8];
    } filelump_t;

    void copyfile(const char *from,const char *to);

    std::fstream m_file;
    std::string m_filepath;
    wadinfo_t m_header;
    std::vector<char*> m_lumps;
    std::vector<filelump_t> m_directorys;
};

}

#endif /*__w_wad_hh__*/
