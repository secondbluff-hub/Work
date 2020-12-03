#pragma once

#include <map>
#include <random>

class QColor;

class ColorGenerate
{
public:
	static QColor generateColor();

	static QColor generateUniqColor(const std::map<int, QColor>& colors);

	static QColor generateContrastColor(int & lastHue,
		const std::map<int, QColor>& colors);

	static bool isContrastColor(const QColor & color, 
		const std::map<int, QColor>& colors, int & num);
	
	static int generateRandom(int min, int max);
};
