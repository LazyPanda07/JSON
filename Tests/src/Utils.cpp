#include "Utils.h"

#include <fstream>

json::JSONParser createParser()
{
	std::ifstream in("data/appends.json");

	return json::JSONParser(in);
}
