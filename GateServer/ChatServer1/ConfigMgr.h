#pragma once
#include <map>
#include <string>

struct SectionInfo
{
	SectionInfo() = default;
	~SectionInfo()
	{
		_section_datas.clear();
	}
	SectionInfo(const SectionInfo& other):_section_datas(other._section_datas){}
	SectionInfo& operator=(const SectionInfo& other)
	{
		if (this == &other)
		{
			return *this;
		}
		_section_datas = other._section_datas;
		return *this;
	}

	std::string operator[](const std::string& key)
	{
		if (_section_datas.find(key) == _section_datas.end())
		{
			return "";
		}
		return _section_datas[key];
	}
	std::map<std::string, std::string> _section_datas;
};

class ConfigMgr
{
public:
	~ConfigMgr();
	SectionInfo operator[](const std::string& section);
	static ConfigMgr& Inst()
	{
		static ConfigMgr cfg_mgr;
		return cfg_mgr;
	}
	ConfigMgr(const ConfigMgr& other);
	ConfigMgr& operator=(const ConfigMgr& other);
private:
	ConfigMgr();
	std::map<std::string, SectionInfo> _config_map;
};

