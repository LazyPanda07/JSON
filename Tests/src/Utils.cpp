#include "Utils.h"

#include <fstream>

json::JsonParser createParser()
{
	std::ifstream in("data/appends.json");

	return json::JsonParser(in);
}
