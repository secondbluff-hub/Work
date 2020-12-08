#pragma once

#include <map>
#include <unordered_set>

#include <QtWidgets/QDialog>

class QListWidget;
class QLineEdit;
class QColor;

class Picker : public QDialog
{
	Q_OBJECT

public:
	Picker(QWidget *parent = Q_NULLPTR);
	explicit Picker(const std::map<int, QColor>& container, QWidget *parent = Q_NULLPTR);

	std::map<int, QColor>&& numbersToColors();

public slots:
	void editListBox();
	void clearListBox();

signals:
	void detectedBadValue(unsigned long long value) const;
	void containerChanged(bool isChaged);

private:
	QLineEdit*				_line;
	QListWidget*			_list;

	std::map<int, QColor>	_lineContainer;

	bool lineParse();
	void insertSingleOrRange(int num, int beginNum);
	void insertValueWithContrastColor(int value);

	void addListItem(int num, const QColor& color);

private slots:
	void provideContextMenu(const QPoint &pos);

	void valueError(unsigned long long value);
	void valueChanged();

	void eraseItem();
	void changeItemColor();
};