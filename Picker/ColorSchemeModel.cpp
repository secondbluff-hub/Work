#include "ColorSchemeModel.h"

ColorSchemeModel::ColorSchemeModel(QWidget *parent)
	: QWidget(parent)
{

}

ColorSchemeModel::~ColorSchemeModel()
{

}

std::vector<std::pair<QString, std::map<int, QColor>>> ColorSchemeModel::chooseScheme()
{
	return std::vector<std::pair<QString, std::map<int, QColor>>>();
}

void ColorSchemeModel::addSequence(DataType data)
{

}