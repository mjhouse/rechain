/*
 * ReChain: The distributed research journal
 * Copyright (C) 2018  Michael House
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Contact: mjhouse@protonmail.com
 *
*/

// system includes
#include <iostream>
#include <string>

// dependency includes
#include <boost/filesystem/path.hpp>
#include <libtorrent/create_torrent.hpp>

// local includes
#include "remote.hpp"
#include "settings.hpp"
#include "logger.hpp"
#include "utility.hpp"

namespace fs = boost::filesystem;
namespace lt = libtorrent;
namespace rc = rechain;

typedef Logger rl;

Remote::Remote( std::shared_ptr<Settings>& s ){
    settings = s;
}

Remote::~Remote(){
    // shut down async stuff
}

void Remote::seed( std::string path ){
    fs::path file(path);
    
    fs::path nfile(settings->gets("files"));
    nfile /= file.filename();

    rc::copy_file( file, nfile, true );
    
    std::string tpath = create_torrent(nfile);

    // start a seeding thread or something. do magic.
}

std::string Remote::create_torrent( fs::path file ){
    // this function is a stub until the distributed
    // mining/publishing is completed
    if(fs::exists(file)){
        // create a torrent
        lt::file_storage storage;
        lt::add_files(storage,file.string());

        lt::create_torrent torrent(storage);
        torrent.add_tracker("http://my.tracker.com/announce");

        // build the torrent path
        fs::path path(settings->gets("torrents"));
        path /= file.filename();
        path.replace_extension(".torrent");

        // remove the filename and set piece hashes
        file.remove_filename();
        lt::set_piece_hashes(torrent,file.string());
      
        // open a file handle and bencode the torrent to it
        std::ofstream ofs(path.string(),std::ios_base::binary);
        lt::bencode(std::ostream_iterator<char>(ofs), torrent.generate());

        return path.string();
    }

    return "";
}

void Remote::leech( std::string t ){
    // stub function
}
