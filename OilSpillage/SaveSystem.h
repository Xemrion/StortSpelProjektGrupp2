#pragma once
#include <string>
#include <map>
#include "vehicle.h"

class SaveSystem
{
private:
	std::map<std::string, int> saveIntergers;
	std::map<std::string, float> saveFloat;
	std::map<std::string, std::string> saveString;
public:
	SaveSystem();
	~SaveSystem();

	void set(const std::string& key, const std::string& value);
	void set(const std::string& key, int value);
	void set(const std::string& key, float value);

	std::string getString(const std::string& key);
	int getInteger(const std::string& key);
	float getFloat(const std::string& key);

	bool existsString(const std::string& key);
	bool existsInteger(const std::string& key);
	bool existsFloat(const std::string& key);

	void read(const std::string& filepath);
	void save(const std::string& filepath);
	void clear();

	static void loadGame(int id, VehicleSlots* slots);
	static void saveGame(VehicleSlots* slots);
	static std::string fileInfo(int id);
};