#include "ColorSchemeModel.h"
#include "ColorSchemesWidget.h"

#include <QStandardItemModel>
#include <QString>
#include <QDebug>

ColorSchemeModel::ColorSchemeModel(const QString& filename, QWidget *parent)
	: _appFile(filename), QWidget(parent)
{
	_model = new QStandardItemModel(this);
	_model->setColumnCount(2);
	_model->setHeaderData(0, Qt::Orientation::Horizontal, "Name");
	_model->setHeaderData(1, Qt::Orientation::Horizontal, "Colors");

	if (!_appFile.open(QIODevice::ReadWrite))
	{
		qDebug() << "Couldn't open: " << filename;
	}
}

ColorSchemeModel::~ColorSchemeModel()
{

}

ColorScheme ColorSchemeModel::chooseScheme()
{
	ColorSchemesWidget w(_appFile, *_model);

	ColorScheme colorScheme;

	if (w.exec() == QDialog::Accepted)
	{
		colorScheme = w.currentScheme();
	}

	return colorScheme;
}

void ColorSchemeModel::addScheme(DataType data)
{

}