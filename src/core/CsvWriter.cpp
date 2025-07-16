#include "../../headers/core/CsvWriter.h"

void 
CsvWriter::SetData(std::vector<int64_t> values, std::string fileName)
{
	std::ofstream ofs(fileName, std::ios::app);
	for (size_t i = 0; i<values.size(); i++)
	{
		ofs << values[i];
		if (i < values.size() - 1)
		{
			ofs << ", ";
		}
		else
		{
			ofs << "\n";
		}
	}
	ofs.close();
}
