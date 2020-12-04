#pragma once

#include <QWidget>
#include <QFile>
#include <QDataStream>

#include <vector>
#include <map>
#include <utility>

class QTreeView;
class QColor;
class QStandardItemModel;
class ColorsDelegate;
class QString;

class ColorSchemesWidget : public QWidget
{
	Q_OBJECT

public:
	using DataType = std::vector<std::pair<QString, std::map<int, QColor>>>;

	ColorSchemesWidget(QWidget *parent = Q_NULLPTR);
	ColorSchemesWidget(const QString& folder, QWidget *parent = Q_NULLPTR);
	~ColorSchemesWidget();

public slots:

signals:
	void dataChanged(bool isChanged = true);

private:
	QTreeView*				_table;
	QStandardItemModel*		_model;
	QFile					_appFile;
	QDataStream				_ioFile;
	DataType				_data;

	bool isUniqName(const QString & name);
	void appendTable();
	void updateFile();

private slots:
	void createKit();
	void clearTable();
	void selectKit();
};
