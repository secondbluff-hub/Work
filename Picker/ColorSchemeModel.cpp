#include "ColorSchemeModel.h"
#include "ColorSchemesWidget.h"

#include <QStandardItemModel>
#include <QString>
#include <QDebug>

ColorSchemeModel::ColorSchemeModel(const QString& filename, QWidget *parent)
	: _appFile(new QFile(filename)), _ioFile(_appFile), QWidget(parent)
{
	_model = new QStandardItemModel(this);
	_model->setColumnCount(2);
	_model->setHeaderData(0, Qt::Orientation::Horizontal, "Name");
	_model->setHeaderData(1, Qt::Orientation::Horizontal, "Colors");

	_isModified = false;

	if (!_appFile->open(QIODevice::ReadWrite))
	{
		qDebug() << "Couldn't open: " << filename;
	}
	else
	{
		if (_appFile->size() > 0)
		{
			if (_ioFile.version() != QDataStream::Qt_5_9)
			{
				qDebug() << "Wrong version of Qt";
			}

			int i;
			_ioFile >> i;

			for (; i > 0; --i)
			{
				QString name;
				_ioFile >> name;

				int j;
				_ioFile >> j;

				std::map<int, QColor> dataColors;
				for (; j > 0; --j)
				{
					int number;
					QString colorName;
					_ioFile >> number >> colorName;

					QColor color(colorName);
					dataColors.insert(std::make_pair(number, color));
				}

				_data.push_back(std::make_pair(name, dataColors));
			}
		}
	}
}

ColorSchemeModel::~ColorSchemeModel()
{
	if (_isModified)
	{
		_appFile->resize(0);

		_ioFile.setVersion(QDataStream::Qt_5_9);

		_ioFile << static_cast<qint32>(_data.size());

		for (int i = 0; i < _data.size(); ++i)
		{
			_ioFile << _data[i].first;
			_ioFile << static_cast<qint32>(_data[i].second.size());

			for (const auto& numToColor : _data[i].second)
			{
				_ioFile << static_cast<qint32>(numToColor.first);
				_ioFile << numToColor.second.name();
			}
		}

		_appFile->flush();
	}

	_appFile->close();
}

ColorScheme ColorSchemeModel::chooseScheme(QWidget* parent)
{
	ColorSchemesWidget w(_data, *_model, parent);

	if (w.exec() == QDialog::Accepted)
	{
		_isModified = _isModified || w.isWindowModified();

		if (_isModified)
		{
			_data = w.forSaving();
		}

		return w.currentScheme();
	}

	return {};
}

void ColorSchemeModel::addScheme(const std::pair<QString, ColorScheme>& scheme)
{
	_isModified = true;
	_data.push_back(scheme);
}