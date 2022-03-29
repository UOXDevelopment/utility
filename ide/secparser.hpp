//Copyright © 2022 Charles Kerr. All rights reserved.

#ifndef secparser_hpp
#define secparser_hpp

#include <cstdint>
#include <string>
#include <filesystem>
class section_t ;
//=========================================================
class secparser_t {
protected:
	auto loadFile(const std::filesystem::path &path)->bool ;
	virtual auto processSection(section_t &section) ->void =0;
	
public:
	virtual ~secparser_t() = default ;
	secparser_t() =default ;
	
	auto load(const std::string &filepath,bool resursive=true,const std::string &extension=".cfg")->bool;
	
};
#endif /* secparser_hpp */
