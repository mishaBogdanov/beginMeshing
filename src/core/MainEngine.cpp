#include "../../headers/core/MainEngine.h"
void MainEngine::BeginExecution()
{
	while (true)
	{
		for (auto& pair : mToExecute)
		{
			pair.second();
		}
	}
}
