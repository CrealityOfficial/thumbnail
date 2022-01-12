#ifndef _IMG2GCODE_H
#define _IMG2GCODE_H
#include<vector>
#include<string>
class  Img2Gcode
	{
	public:
		Img2Gcode();
		~Img2Gcode();

		static bool imgEncode(const std::vector<unsigned char>& prevData, std::vector<std::string>& encodeData, const std::string& imgSizes, 
			const std::string& imgFormat, const int& layerCount, const char* saveFile);
		static bool imgDecode(std::vector<std::string>& prevData, std::vector<unsigned char>& decodeData);
	};
#endif