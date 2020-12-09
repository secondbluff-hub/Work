#pragma once

#include <QWidget>
#include <QFile>

#include <vector>
#include <map>
#include <utility>

class QString;
class QStandardItemModel;

using ColorScheme = std::map<int, QColor>;

class ColorSchemeModel : public QWidget
{
	Q_OBJECT

public:
	using DataType = std::pair<QString, ColorScheme>;

	ColorSchemeModel(const QString& filename, QWidget *parent = Q_NULLPTR);
	~ColorSchemeModel();

public slots:
	ColorScheme chooseScheme();

signals:


private:
	QStandardItemModel*		_model;
	QFile					_appFile;

private slots:
	void addScheme(DataType data);
};
