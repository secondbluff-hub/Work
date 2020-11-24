#pragma once

#include <QtWidgets/QDialog>

#include <set>

class QLineEdit;
class QPushButton;
class QListWidget;
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

private:
	QLineEdit*		_line;
	QPushButton*	_genButton;
	QPushButton*	_clearButton;
	QListWidget*	_list;
	QVBoxLayout*	_vbox;
	std::set<int>	_lineContainer;

	void lineParse();
	void insertSingleOrRange(int num, int beginNum);

private slots:
	void provideContextMenu(const QPoint &pos);
};
