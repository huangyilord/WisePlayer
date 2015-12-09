#ifndef __FORMAT_CONVERT_
#define __FORMAT_CONVERT_

#include <vector>

namespace Util
{
	class FormatConvert
	{
	public:
		// convert the ascii float array to binary float array
		// float array are "," separated.
		// asciiSize is the length of string which does not contain '\0'
		static bool ASCIIFloatArrayToBinFloatArray(char* asciiArray, unsigned int asciiSize, std::vector<float>& binArray);
		
		static BOOL IPv4Str2IPByteArray(const char* ipStr, BYTE* pByteArray, UINT32 byteArraySize);

	};
}

#endif
