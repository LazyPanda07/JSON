#include "WrongInputStreamException.h"

#include <fstream>

using namespace std;

namespace json
{
	namespace exceptions
	{
		string WrongInputStreamException::getErrorMessage(const istream& stream)
		{
			string errorMessage;

			if (stream.fail())
			{
				errorMessage += "Fail to read from input stream\n";
			}

			if (stream.eof())
			{
				errorMessage += "End of stream\n";
			}

			if (stream.bad())
			{
				errorMessage += "Error in input stream\n";
			}

			if (stream.good())
			{
				errorMessage = "All good";
			}

			return errorMessage;
		}

		WrongInputStreamException::WrongInputStreamException(const istream& stream) :
			BaseJSONException(WrongInputStreamException::getErrorMessage(stream))
		{
					
		}
	}
}
