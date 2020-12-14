#pragma once

#include <map>
#include <utility>

class QColor;
class QWidget;
class QString;

class IColorSchemeStorage
{
public:
	virtual ~IColorSchemeStorage() {};

	virtual std::map<int, QColor> chooseScheme(QWidget* parent) = 0;

	virtual void addScheme(const std::pair<QString, std::map<int, QColor>>& scheme) = 0;
};
