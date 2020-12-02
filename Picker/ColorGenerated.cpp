#include "ColorGenerated.h"

#include <random>

#include <QColor>

QColor ColorGenerated::generateColor()
{
	return QColor::fromHsv	(	generateRandom(0, 359),
								generateRandom(200, 255),
								generateRandom(200, 255)
							);
}

QColor ColorGenerated::generateUniqColor(const std::map<int, QColor>& colors)
{
	//static int minRange = 75;
	//static int maxRange = 360 - minRange / 2;
	auto isUniqColor = [&colors = colors](const QColor& color)
	{
		for (const auto& numToColor : colors)
		{
			if (numToColor.second == color) {
				return false;
			}
		}
		return true;
	};

	QColor color;

	do
	{
		color = generateColor();
	} while (!isUniqColor(color));

	return color;
}

QColor ColorGenerated::generateContrastColor(int & lastHue,
												const std::map<int, QColor>& colors)
{
	QColor color = generateColor();

	if (colors.size() <= 30 && colors.size() > 0)
	{
		int hue = color.hue();
		int step = 144;
		hue = (lastHue + step);
		if ((colors.size() + 1) % 5 == 0)
		{
			hue += 12;
		}
		hue %= 360;
		color.setHsv(hue, color.saturation(), color.value());
	}
	else
	{
		color = generateUniqColor(colors);
	}

	lastHue = color.hue();

	return color;
}

bool ColorGenerated::isContrastColor(const QColor & color,
											const std::map<int, QColor>& colors, int & num)
{
	int minRange = 360 / (colors.size() + 1);
	int maxRange = 360 - minRange / 2;

	for (const auto numToColor : colors)
	{
		if (abs(color.hue() - numToColor.second.hue()) < minRange ||
			abs(color.hue() - numToColor.second.hue()) > maxRange)
		{
			num = numToColor.first;
			return false;
		}
	}
	return true;
}

int ColorGenerated::generateRandom(int min, int max)
{
	std::random_device device;
	std::mt19937 mt_rand(device());

	std::uniform_int_distribution<int> range(min, max);
	return range(mt_rand);
}
