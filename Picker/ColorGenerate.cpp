#include "ColorGenerate.h"

#include <random>

#include <QColor>

QColor ColorGenerate::generateColor()
{
	return QColor::fromHsv	(	generateRandom(0, 359),
								generateRandom(200, 255),
								generateRandom(200, 255)
							);
}

QColor ColorGenerate::generateUniqColor(const std::map<int, QColor>& colors)
{
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

QColor ColorGenerate::generateContrastColor(int & lastHue,
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

bool ColorGenerate::isContrastColor(const QColor & color,
											const std::map<int, QColor>& colors, int & num)
{
	int minRange = 360 / (colors.size());
	int maxRange = 360 - minRange / 2;

	for (const auto numToColor : colors)
	{
		auto range = abs(color.hue() - numToColor.second.hue());

		if (range < minRange || range > maxRange)
		{
			num = numToColor.first;

			return false;
		}
	}

	return true;
}

int ColorGenerate::generateRandom(int min, int max)
{
	std::random_device device;
	std::mt19937 mt_rand(device());

	std::uniform_int_distribution<int> range(min, max);
	return range(mt_rand);
}
