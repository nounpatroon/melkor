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
    m_file = new std::fstream;
    m_filepath = new std::string;
    m_header = new wadinfo_t;
    m_lump = new std::vector<void*>;
    m_directory = new std::vector<filelump_t>;
}

wad_c::~wad_c() {
    delete(m_file);
    delete(m_filepath);
    delete(m_header);
    delete(m_directory);
}

void wad_c::open(const char *path) {
    m_file->close();
    m_file->open(path, std::ios::in | std::ios::out | std::ios::binary);
    m_filepath->assign(path);
    
    if(m_file->fail()) {
        fclose(fopen(path, "wb+"));
        m_file->open(path, std::ios::in | std::ios::out | std::ios::binary);

        snprintf(m_header->identification, 5, "%s", "PWAD");
        m_header->infotableofs = sizeof(wadinfo_t);
        m_header->numlumps = 0;

        m_file->write((char*)m_header, sizeof(wadinfo_t));
    }
    else {
        // read header
        m_file->seekg(0, std::ios::beg);
        m_file->read((char*)m_header, sizeof(wadinfo_t));

        // read directory
        m_directory->resize(m_header->numlumps);
        m_file->seekg(m_header->infotableofs, std::ios::beg);
        for(uint32_t i = 0;i < m_header->numlumps;i++) {
            m_file->read((char*)&m_directory->at(i), sizeof(filelump_t));
        }

        // read lump
        m_lump->resize(m_header->numlumps);
        m_file->seekg(sizeof(wadinfo_t), std::ios::beg);
        for(uint32_t i = 0;i < m_header->numlumps;i++) {
            m_lump->at(i) = ::operator new(m_directory->at(i).size);

            m_file->read((char*)m_lump->at(i), m_directory->at(i).size);
        }
    }

    m_file->close();
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

uint32_t wad_c::get_total() {
    return m_header->numlumps;
}

void* wad_c::get_data(uint32_t index) {
    if(index >= m_header->numlumps)
        return NULL;

    return m_lump->at(index);
}

char* wad_c::get_name(uint32_t index) {
    if(index >= m_header->numlumps)
        return NULL;

    return m_directory->at(index).name;
}

uint32_t wad_c::get_size(uint32_t index) {
    if(index >= m_header->numlumps)
        return 0;

    return m_directory->at(index).size;
}

void wad_c::insert(uint32_t index, const char* name, void *data, size_t size) {
    if(index > m_header->numlumps)
        return;
    if(index == m_header->numlumps) {
        m_directory->resize(m_directory->size()+1);
        m_lump->resize(m_lump->size()+1);
    }

    m_file->open(m_filepath->c_str(), std::ios::in | std::ios::out | std::ios::binary);
    filelump_t newdirectory;

    // add to list
    snprintf(newdirectory.name, 9, "%-8.8s", name);
    for(int i = 0;i < 8;i++)
        newdirectory.name[i] = toupper(newdirectory.name[i]);
    newdirectory.size = size;
    newdirectory.filepos = m_directory->at(index).filepos;

    // add data
    m_directory->insert(m_directory->begin() + index, newdirectory);
    m_lump->insert(m_lump->begin() + index, data);

    // write data
    m_file->seekg(0, std::ios::beg);
    m_file->write((char*)m_header, sizeof(wadinfo_t));
    for(uint32_t i = 0;i < (m_header->numlumps+1);i++) {
        m_directory->at(i).filepos = m_file->tellg();
        m_file->write((char*)m_lump->at(i), m_directory->at(i).size);
    }
    
    // update header
    m_header->numlumps += 1;
    m_header->infotableofs = m_file->tellg();
    m_file->seekg(0, std::ios::beg);
    m_file->write((char*)m_header, sizeof(wadinfo_t));

    // write directory
    m_file->seekg(m_header->infotableofs, std::ios::beg);
    for(uint32_t i = 0;i < m_header->numlumps;i++) {
        m_file->write((char*)&m_directory->at(i), sizeof(filelump_t));
    }

    // end
    m_file->close();
}

void wad_c::erase(uint32_t index) {
    if(index > m_header->numlumps || m_header->numlumps == 0)
        return;

    std::fstream *wadtmpfile = new std::fstream;
    std::ofstream { ".s4d3dhayuwjsw.tmp" };

    // open files
    wadtmpfile->open(".s4d3dhayuwjsw.tmp", std::ios::in | std::ios::out | std::ios::binary);

    // delete data
    m_directory->erase(m_directory->begin() + index);
    m_lump->erase(m_lump->begin() + index);

    // write to temporal
    wadtmpfile->seekg(0, std::ios::beg);
    wadtmpfile->write((char*)m_header, sizeof(wadinfo_t));
    for(uint32_t i = 0;i < (m_header->numlumps-1);i++) {
        m_directory->at(i).filepos = wadtmpfile->tellg();
        wadtmpfile->write((char*)m_lump->at(i), m_directory->at(i).size);
    }

    // update header
    m_header->numlumps -= 1;
    m_header->infotableofs = wadtmpfile->tellg();
    wadtmpfile->seekg(0, std::ios::beg);
    wadtmpfile->write((char*)m_header, sizeof(wadinfo_t));

    // write directory
    wadtmpfile->seekg(m_header->infotableofs, std::ios::beg);
    for(uint32_t i = 0;i < m_header->numlumps;i++) {
        wadtmpfile->write((char*)&m_directory->at(i), sizeof(filelump_t));
    }

    // end
    wadtmpfile->close();
    copyfile(".s4d3dhayuwjsw.tmp", m_filepath->c_str());
    remove(".s4d3dhayuwjsw.tmp");
}

void wad_c::set_id(const char *mode) {
    m_file->open(m_filepath->c_str(), std::ios::in | std::ios::out | std::ios::binary);
    
    if(!strcmp(mode, "IWAD")) {
        snprintf(m_header->identification, 5, "%s", "IWAD");
    }
    else if(!strcmp(mode, "PWAD")) {
        snprintf(m_header->identification, 5, "%s", "PWAD");
    }
    else {
        return;
    }

    m_file->seekg(0, std::ios::beg);
    m_file->write((char*)m_header, sizeof(wadinfo_t));

    m_file->close();
}

const char* wad_c::get_id() {
    return m_header->identification;
}

const char* wad_c::get_path() {
    return m_filepath->c_str();
}
