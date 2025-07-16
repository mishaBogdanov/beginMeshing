#pragma once
#include <vector>
#include <string>
#include <filesystem> 
#include <cstdint>
#include <fstream>

class CsvWriter
{
public:
	static void SetData(std::vector<int64_t> values, std::string fileName);
};

