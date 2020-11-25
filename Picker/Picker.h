#pragma once

#include <QtWidgets/QDialog>
#include <QListWidget>

#include <map>

class QLineEdit;
class QPushButton;
class QVBoxLayout;

class Picker : public QDialog
{
	Q_OBJECT

public:
	Picker(QWidget *parent = Q_NULLPTR);

public slots:
	void editListBox();
	void clearListBox();

signals:
	void detectedBadValue(long long value);

private:
	QLineEdit*		_line;
	QPushButton*	_genButton;
	QPushButton*	_clearButton;
	QListWidget*	_list;
	QVBoxLayout*	_vbox;
	std::map<int, Qt::GlobalColor>	_lineContainer;

	bool lineParse();
	void insertSingleOrRange(int num, int beginNum);
	void insertValueWithRandomColor(int value);

private slots:
	void provideContextMenu(const QPoint &pos);
	void valueError(long long value);
	void valueRight();
};
