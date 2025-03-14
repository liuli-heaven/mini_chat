#include "ConfigMgr.h"

#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

ConfigMgr::~ConfigMgr()
{
	_config_map.clear();
}

ConfigMgr::ConfigMgr()
{
	boost::filesystem::path current_path = boost::filesystem::current_path();
	std::string ini_path = (current_path / "config.ini").string();
	std::cout << "config_ini_path is: " << ini_path << "\n";

	boost::property_tree::ptree p;
	boost::property_tree::ini_parser::read_ini(ini_path, p);

	for(auto&& section_pair : p)
	{
		
		std::string section_name = section_pair.first;
		boost::property_tree::ptree section_infos = section_pair.second;
		SectionInfo section;
		for(auto&& section_info : section_infos)
		{
			std::string key = section_info.first;
			std::string value = section_info.second.get_value<std::string>();
			section._section_datas[key] = value;
		}
		_config_map[section_name] = section;
	}

	for (const auto& section_entry : _config_map) {
		const std::string& section_name = section_entry.first;
		SectionInfo section_config = section_entry.second;
		std::cout << "[" << section_name << "]" << "\n";
		for (const auto& key_value_pair : section_config._section_datas) {
			std::cout << key_value_pair.first << "=" << key_value_pair.second << "\n";
		}
	}
}

ConfigMgr::ConfigMgr(const ConfigMgr& other)
{
	_config_map = other._config_map;
}

ConfigMgr& ConfigMgr::operator=(const ConfigMgr& other)
{
	if (&other == this)
	{
		return *this;
	}
	_config_map = other._config_map;
	return *this;
}

SectionInfo ConfigMgr::operator[](const std::string& section)
{
	if (_config_map.find(section) == _config_map.end())
	{
		return SectionInfo();
	}
	return _config_map[section];
}

