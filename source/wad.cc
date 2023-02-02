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

#include <iostream>
#include <fcntl.h>   // open
#include <unistd.h>  // read, write, close
#include <cstdio>    // BUFSIZ

#include "wad.hh"

wad_c::wad_c() {

}

wad_c::~wad_c() {
    for(uint32_t i = 0;i < m_header.numlumps;i++) {
        free(m_lumps[i]);
    }
}

void wad_c::open(const char *path) {
    m_file.close();
    m_file.open(path, std::ios::in | std::ios::out | std::ios::binary);
    m_filepath.assign(path);
    
    if(m_file.fail()) {
        fclose(fopen(path, "wb+"));
        m_file.open(path, std::ios::in | std::ios::out | std::ios::binary);

        snprintf(m_header.identification, 5, "%s", "PWAD");
        m_header.infotableofs = sizeof(wadinfo_t);
        m_header.numlumps = 0;

        m_file.write((char*)&m_header, sizeof(wadinfo_t));
    }
    else {
        // read header
        m_file.seekg(0, std::ios::beg);
        m_file.read((char*)&m_header, sizeof(wadinfo_t));

        // read directory
        m_directorys.resize(m_header.numlumps);
        m_file.seekg(m_header.infotableofs, std::ios::beg);
        for(uint32_t i = 0;i < m_header.numlumps;i++) {
            m_file.read((char*)&m_directorys[i], sizeof(filelump_t));
        }

        // read lump
        m_lumps.resize(m_header.numlumps);
        m_file.seekg(sizeof(wadinfo_t), std::ios::beg);
        for(uint32_t i = 0;i < m_header.numlumps;i++) {
            m_lumps[i] = (char*)malloc(m_directorys[i].size);
            m_file.read((char*)m_lumps[i], m_directorys[i].size);
        }
    }

    m_file.close();
}

void wad_c::copyfile(const char *from, const char *to) {
    char buf[BUFSIZ];
    size_t size;

    FILE* source = fopen(from, "rb");
    FILE* dest = fopen(to, "wb");

    // clean and more secure
    // feof(FILE* stream) returns non-zero if the end of file indicator for stream is set

    while (size = fread(buf, 1, BUFSIZ, source)) {
        fwrite(buf, 1, size, dest);
    }

    fclose(source);
    fclose(dest);
}

uint32_t wad_c::get_numlumps() {
    return m_header.numlumps;
}

const char* wad_c::get_lump_data(const uint32_t index) {
    if(index >= m_header.numlumps)
        return NULL;

    return m_lumps[index];
}

const char* wad_c::get_directory_name(const uint32_t index) {
    if(index >= m_header.numlumps)
        return NULL;

    return m_directorys[index].name;
}

uint32_t wad_c::get_directory_size(const uint32_t index) {
    if(index >= m_header.numlumps)
        return 0;

    return m_directorys[index].size;
}

void wad_c::insert(const uint32_t index, const char* name, char *data, const size_t size) {
    if(index > m_header.numlumps)
        return;
    if(index == m_header.numlumps) {
        m_directorys.resize(m_directorys.size()+1);
        m_lumps.resize(m_lumps.size()+1);
    }

    m_file.open(m_filepath.c_str(), std::ios::in | std::ios::out | std::ios::binary);
    filelump_t newdirectory;

    // add to list
    snprintf(newdirectory.name, 9, "%-8.8s", name);
    for(int i = 0;i < 8;i++)
        newdirectory.name[i] = toupper(newdirectory.name[i]);
    newdirectory.size = size;
    newdirectory.filepos = m_directorys[index].filepos;

    // add data
    m_directorys.insert(m_directorys.begin() + index, newdirectory);
    m_lumps.insert(m_lumps.begin() + index, data);
    m_lumps[index] = (char*)malloc(m_directorys[index].size);
    memcpy(m_lumps[index], data, size);

    // write data
    m_file.seekg(0, std::ios::beg);
    m_file.write((char*)&m_header, sizeof(wadinfo_t));
    for(uint32_t i = 0;i < (m_header.numlumps+1);i++) {
        m_directorys[i].filepos = m_file.tellg();
        m_file.write((char*)m_lumps[i], m_directorys[i].size);
    }
    
    // update header
    m_header.numlumps += 1;
    m_header.infotableofs = m_file.tellg();
    m_file.seekg(0, std::ios::beg);
    m_file.write((char*)&m_header, sizeof(wadinfo_t));

    // write directory
    m_file.seekg(m_header.infotableofs, std::ios::beg);
    for(uint32_t i = 0;i < m_header.numlumps;i++) {
        m_file.write((char*)&m_directorys[i], sizeof(filelump_t));
    }

    // end
    m_file.close();
}

void wad_c::erase(const uint32_t index) {
    if(index > m_header.numlumps || m_header.numlumps == 0)
        return;

    std::fstream wadtmpfile;
    std::ofstream { ".s4d3dhayuwjsw.tmp" };

    // open files
    wadtmpfile.open(".s4d3dhayuwjsw.tmp", std::ios::in | std::ios::out | std::ios::binary);

    // delete data
    m_directorys.erase(m_directorys.begin() + index);
    free(m_lumps[index]);
    m_lumps.erase(m_lumps.begin() + index);

    // write to temporal
    wadtmpfile.seekg(0, std::ios::beg);
    wadtmpfile.write((char*)&m_header, sizeof(wadinfo_t));
    for(uint32_t i = 0;i < (m_header.numlumps-1);i++) {
        m_directorys[i].filepos = wadtmpfile.tellg();
        wadtmpfile.write((char*)m_lumps[i], m_directorys[i].size);
    }

    // update header
    m_header.numlumps -= 1;
    m_header.infotableofs = wadtmpfile.tellg();
    wadtmpfile.seekg(0, std::ios::beg);
    wadtmpfile.write((char*)&m_header, sizeof(wadinfo_t));

    // write directory
    wadtmpfile.seekg(m_header.infotableofs, std::ios::beg);
    for(uint32_t i = 0;i < m_header.numlumps;i++) {
        wadtmpfile.write((char*)&m_directorys[i], sizeof(filelump_t));
    }

    // end
    wadtmpfile.close();
    copyfile(".s4d3dhayuwjsw.tmp", m_filepath.c_str());
    remove(".s4d3dhayuwjsw.tmp");
}

void wad_c::set_identification(const char *mode) {
    m_file.open(m_filepath.c_str(), std::ios::in | std::ios::out | std::ios::binary);
    
    if(!strcmp(mode, "IWAD")) {
        snprintf(m_header.identification, 5, "%s", "IWAD");
    }
    else if(!strcmp(mode, "PWAD")) {
        snprintf(m_header.identification, 5, "%s", "PWAD");
    }
    else {
        return;
    }

    m_file.seekg(0, std::ios::beg);
    m_file.write((char*)&m_header, sizeof(wadinfo_t));

    m_file.close();
}

const char* wad_c::get_identification() {
    return m_header.identification;
}

const char* wad_c::get_path() {
    return m_filepath.c_str();
}
