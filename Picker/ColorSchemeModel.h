#pragma once

#include "IColorSchemeStorage.h"

#include <QWidget>
#include <QFile>

#include <vector>
#include <map>
#include <utility>

class QString;
class QStandardItemModel;

using ColorScheme = std::map<int, QColor>;

class ColorSchemeModel : public QWidget, public IColorSchemeStorage
{
	Q_OBJECT

public:
	using DataType = std::vector<std::pair<QString, ColorScheme>>;

	ColorSchemeModel(const QString& filename, QWidget *parent = nullptr);
	~ColorSchemeModel();

public slots:
	ColorScheme chooseScheme(QWidget* parent) override;

	void addScheme(const std::pair<QString, ColorScheme>& scheme) override;

private:
	QStandardItemModel*		_model;
	QFile					_appFile;
	QDataStream				_ioFile;
	DataType				_data;
	bool					_isModified;
};
