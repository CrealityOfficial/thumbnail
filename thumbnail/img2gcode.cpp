#include"img2gcode.h"

#include<fstream>

Img2Gcode::Img2Gcode()
{

}
Img2Gcode::~Img2Gcode()
{

}

int getLineStart(const std::vector<unsigned char>& prevData, int line_strlen)
{
	int LineStart = 0;
	int pixelNum = prevData.size();
	int linePixel = line_strlen / 4 * 3;
	int startNo = 0;
	for (int i = 0; i < pixelNum; i++)
	{
		if (prevData[i] != 0)
		{
			startNo = i;
			break;
		}
	}
	LineStart = startNo / linePixel;
	return LineStart;
}

int getLineEnd(const std::vector<unsigned char>& prevData, int line_strlen)
{
	int LineEnd = 0;
	int pixelNum = prevData.size();
	int linePixel = line_strlen / 4 * 3;
	int endNo = pixelNum - 1;
	for (int i = endNo; i >= 0; i--)
	{
		if (prevData[i] != 0)
		{
			endNo = i;
			break;
		}
	}
	LineEnd = endNo / linePixel;
	return LineEnd;
}

int getImageHeight(const std::string str)
{
	int val = 0;
	std::string str2 = "";
	for (int i = str.length() - 1; i >= 0; i--) {
		if (str.at(i) >= 48 && str.at(i) < 58) {
			str2 += str.at(i);
		}
		else break;
	}
	int Dec = 1;
	for (int i = 0; i < str2.length(); i++) {
		val += (int(str2.at(i)) - 48) * Dec;
		Dec *= 10;
	}
	return val;
}

static const std::string base64_chars =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz"
	"0123456789+/";


static inline bool is_base64(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_encode(std::vector<unsigned char> const& buffer) {
	size_t in_len = buffer.size();
	unsigned char const* bytes_to_encode = buffer.data();
	std::string ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (in_len--) {
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (i = 0; (i < 4); i++)
				ret += base64_chars[char_array_4[i]];
			i = 0;
		}
	}

	if (i)
	{
		for (j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

		for (j = 0; (j < i + 1); j++)
			ret += base64_chars[char_array_4[j]];

		while ((i++ < 3))
			ret += '=';

	}

	return ret;

}

std::vector<unsigned char> base64_decode(std::string const& encoded_string) {
	size_t in_len = encoded_string.size();
	int i = 0;
	int j = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];
	std::vector<unsigned char> ret;

	while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
		char_array_4[i++] = encoded_string[in_]; in_++;
		if (i == 4) {
			for (i = 0; i < 4; i++)
				char_array_4[i] = base64_chars.find(char_array_4[i]) & 0xff;

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
				ret.push_back(char_array_3[i]);
			i = 0;
		}
	}

	if (i) {
		for (j = 0; j < i; j++)
			char_array_4[j] = base64_chars.find(char_array_4[j]) & 0xff;

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);

		for (j = 0; (j < i - 1); j++) ret.push_back(char_array_3[j]);
	}

	return ret;
}


bool Img2Gcode::imgEncode(const std::vector<unsigned char>& prevData, std::vector<std::string>& encodeData, const std::string& imgSizes,
	const std::string& imgFormat, const int& layerCount, const char* saveFile)
{
	bool writefile = (saveFile==nullptr)?false:true;
	std::string prevData_base64 = base64_encode(prevData);
	int prevDataSize = prevData.size();
	int mainDataSize = prevData_base64.size();
	int line_strlen = 76;
	int lineNum = mainDataSize % line_strlen == 0 ? mainDataSize / line_strlen : mainDataSize / line_strlen + 1;
	lineNum += 2;//Í·Î²Á½ÐÐ
	int imgDataStartLineNo = getImageHeight(imgSizes) * 0.0167 + 0.5;
	int imgDataEndLineNo = getImageHeight(imgSizes) - imgDataStartLineNo;
	std::string headData = imgFormat + std::string(" begin ") + imgSizes + std::string(" ") + std::to_string(prevDataSize) + std::string(" ") +
		std::to_string(imgDataStartLineNo) + std::string(" ") + std::to_string(imgDataEndLineNo) + std::string(" ") + std::to_string(layerCount);
	std::string endData = imgFormat + std::string(" end");

	encodeData.reserve(lineNum);
	std::ofstream outfile;
	if (writefile)
	{
		outfile.open(saveFile, std::ios::binary | std::ios::trunc | std::ios::in | std::ios::out);
	}
	for (int i = 0; i < lineNum; i++)
	{
		std::string CurLine;
		if (i == 0) {
			CurLine = "; " + headData;
		}
		else if (i == lineNum - 1) {
			CurLine = "; " + endData;
		}
		else if (i == lineNum - 2) {
			CurLine = "; " + std::string(prevData_base64, line_strlen * (i - 1));
		}
		else {
			CurLine = "; " + std::string(prevData_base64, line_strlen * (i - 1), line_strlen);
		}
		if (writefile)
		{
			outfile << CurLine << std::endl;
		}
		encodeData.push_back(CurLine);
	}
	if (writefile)
	{
		outfile.close();
	}
	return true;
}

bool Img2Gcode::imgDecode(std::vector<std::string>& prevEncodeData, std::vector<unsigned char>& decodeData)
{
	std::string prevData_base64;
	int linesNum = prevEncodeData.size();
	for (int i = 1; i < linesNum - 1; i++)
	{
		std::string curLineData = std::string(prevEncodeData[i], 2);
		prevData_base64 += curLineData;
	}
	decodeData = base64_decode(prevData_base64);
	return true;
}
