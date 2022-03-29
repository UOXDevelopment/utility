//Copyright © 2022 Charles Kerr. All rights reserved.

#include "secparser.hpp"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <optional>
#include "section.hpp"

using namespace std::string_literals ;

//=========================================================
// Load directories/files
//=========================================================
//=========================================================
auto secparser_t::loadFile(const std::filesystem::path &path)->bool{
	auto input = std::ifstream(path.string()) ;
	auto rvalue = false ;
	if (input.is_open()){
		int line_number = 0 ;
		try {
			while (input.good() && !input.eof()) {
				
				auto sec = section_t::load(input, line_number, path.string()) ;
				if (sec) {
					// it is ok
					processSection(sec.value()) ;
				}
				
			}
			rvalue = true ;
		} catch (const std::runtime_error &e) {
			// we aren't going to continue
			std::cerr <<e.what()<<std::endl;
			
		}
	}
	return rvalue ;
}
//=========================================================
auto secparser_t::load(const std::string &filepath,bool recursive,const std::string &extension)->bool{
	auto path = std::filesystem::path(filepath) ;
	auto rvalue = false ;
	if (std::filesystem::is_directory(path)){
		// we are going to search through
		auto diter = std::filesystem::directory_iterator(path) ;
		for (const auto &entry : diter){
			// First, is this a directory or a file?
			if (entry.is_directory() && recursive){
				load(entry.path().string(),recursive,extension);
			}
			else if (entry.is_regular_file() && (entry.path().extension()==extension)){
				// A file, we need to process this
				if (!loadFile(entry.path())) {
					std::cerr <<"Unable to process "<<path.string()<<std::endl;
				}
			}
		}
		rvalue = true ;
	}
	else if (std::filesystem::is_regular_file(path)){
		// Process the file
		// A file, we need to process this
		rvalue= loadFile(path);
	}
	return rvalue ;
}
