#pragma once

#include <QWidget>

#include <map>

class QTreeView;
class QColor;

class ColorSchemesWidget : public QWidget
{
	Q_OBJECT

public:
	ColorSchemesWidget(QWidget *parent = Q_NULLPTR);
	ColorSchemesWidget(const QString& folder, QWidget *parent = Q_NULLPTR);
	~ColorSchemesWidget();

	void insertData(std::map<int, QColor>&& newData);

public slots:
	void createKit();
	void clearTable();
	void selectKit();

signals:
	void dataChanged(bool isChanged = true);

private:
	QTreeView*				_table;
	std::map<int, QColor>	_data;

private slots:

};
