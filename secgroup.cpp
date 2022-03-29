//Copyright © 2022 Charles Kerr. All rights reserved.

#include "secgroup.hpp"

#include <iostream>
#include <fstream>
#include <optional>
#include <stdexcept>

#include "strutil.hpp"

//=========================================================
// constructor
//=========================================================
//=========================================================
secgroup_t::secgroup_t():secparser_t(){
	
}
//=========================================================
secgroup_t::secgroup_t(const std::string &filepath,bool resursive,const std::string &extension) :secgroup_t() {
	if (!filepath.empty()){
		load(filepath,resursive,extension);
	}
}

//=========================================================
// processSection
//========================================================
//=========================================================
auto secgroup_t::processSection(section_t &section) ->void {
	_section_types[section.header().type()][section.header().identifier()]=std::move(section);

}

//=========================================================
// Load directories/files
//=========================================================
//=========================================================
/*
auto secgroup_t::loadFile(const std::filesystem::path &path)->bool{
	auto input = std::ifstream(path.string()) ;
	auto rvalue = false ;
	if (input.is_open()){
		int line_number = 0 ;
		try {
			while (input.good() && !input.eof()) {
				
				auto sec = section_t::load(input, line_number, path.string()) ;
				if (sec) {
					// it is ok
					_section_types[sec.value().header().type()][sec.value().header().identifier()]=std::move(sec.value());
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
auto secgroup_t::load(const std::string &filepath,bool recursive,const std::string &extension)->bool{
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
*/
//=========================================================
// Entry mainpulate
//========================================================

//=========================================================
auto secgroup_t::size() const ->size_t {
	return _section_types.size();
}
//=========================================================
auto secgroup_t::size(const std::string &type) const ->size_t {
	auto iter = _section_types.find(type);
	if (iter != _section_types.end()){
		return iter->second.size() ;
	}
	return 0 ;
}

//=========================================================
auto secgroup_t::totalSections() const ->size_t {
	auto total = size_t(0) ;
	for (const auto & entry : _section_types){
		total += entry.second.size();
	}
	return total ;
}

//=========================================================
auto secgroup_t::contains(const std::string &type) const->bool {
	return _section_types.find(type)!=_section_types.end();
}

//=========================================================
auto secgroup_t::contains(const std::string &type, const std::string &identifier) const ->bool {
	auto iter = _section_types.find(type);
	if (iter != _section_types.end()) {
		auto siter = iter->second.find(identifier);
		return siter != iter->second.end();
	}
	return false ;
}

//=========================================================
auto secgroup_t::remove(const std::string &type, const std::string &identifier) ->bool {
	auto rvalue = false ;
	auto iter = _section_types.find(type) ;
	if (iter != _section_types.end()) {
		auto siter = iter->second.find(identifier)  ;
		if (siter != iter->second.end()){
			iter->second.erase(siter);
			rvalue = true ;
		}
	}
	return rvalue ;
}
//=========================================================
auto secgroup_t::remove(const std::string &type) ->bool {
	auto rvalue = false ;
	auto iter = _section_types.find(type) ;
	if (iter != _section_types.end()){
		_section_types.erase(iter) ;
		rvalue = true ;
	}
	return rvalue ;
}

//=========================================================
auto secgroup_t::type(const std::string &type) ->std::unordered_map<std::string, section_t>* {
	auto iter = _section_types.find(type);
	if (iter != _section_types.end()){
		return &iter->second;
	}
	return nullptr;
}

//=========================================================
auto secgroup_t::type(const std::string &type) const ->const std::unordered_map<std::string, section_t>* {
	auto iter = _section_types.find(type);
	if (iter != _section_types.end()){
		return &iter->second;
	}
	return nullptr ;
}

//=========================================================
auto secgroup_t::section(const std::string &type,const std::string &identifier) ->section_t* {
	auto iter = _section_types.find(type);
	if (iter != _section_types.end()){
		auto siter = iter->second.find(identifier);
		if (siter != iter->second.end()){
			return &siter->second;
		}
	}
	return nullptr ;
}
//=========================================================
auto secgroup_t::section(const std::string &type,const std::string &identifier)const ->const section_t* {
	auto iter = _section_types.find(type);
	if (iter != _section_types.end()){
		auto siter = iter->second.find(identifier);
		if (siter != iter->second.end()){
			return &siter->second;
		}
	}
	return nullptr ;
}
//=========================================================
auto secgroup_t::normalize(const std::string &type) ->void {
	auto iter = _section_types.find(type) ;
	if (iter != _section_types.end()){
		for (auto &sec : iter->second) {
			if (sec.second.needsNormalization()){
				// We need to find this one
				normalize(sec.second);
			}
		}
	}
}
//=========================================================
auto secgroup_t::normalize(section_t &section) ->void {
	auto parent = section.header().parent() ;
	auto type = section.header().type() ;
	auto parsec = this->section(type, parent) ;
	if (parsec->needsNormalization()){
		normalize(*parsec);
	}
	// we need to append the keyvalues that are not present on the original
	for (auto &entry : parsec->entries()){
		if (section.first(entry.key())!= nullptr) {
			// We need to add this one
			auto keyvalue = entry ; // we make a copy, becasue the pushback moves the entry
			section.push_back(keyvalue) ;
		}
	}
	section.normalized = true ;
	
}
