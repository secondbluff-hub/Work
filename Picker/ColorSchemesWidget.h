#pragma once

#include <QWidget>

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
	ColorSchemesWidget(QWidget *parent = Q_NULLPTR);
	ColorSchemesWidget(const QString& folder, QWidget *parent = Q_NULLPTR);
	~ColorSchemesWidget();

public slots:
	void createKit();
	void clearTable();
	void selectKit();

	void editTable();

signals:
	void dataChanged(bool isChanged = true);

private:
	QTreeView*												_table;
	QStandardItemModel*										_model;
	ColorsDelegate*											_delegate;
	std::vector<std::pair<QString, std::map<int, QColor>>>	_data;

private slots:

};
