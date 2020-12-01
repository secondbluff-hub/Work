#pragma once

#include <QtWidgets/QDialog>

#include <map>
#include <unordered_set>
#include <random>

class QListWidget;
class QLineEdit;
class QColor;

template <typename T>
int generateRandom(T& mt_rand, int min, int max);

class Picker : public QDialog
{
	Q_OBJECT

public:
	Picker(QWidget *parent = Q_NULLPTR);

	std::map<int, QColor> numbersToColors() const;

public slots:
	void editListBox();
	void clearListBox();

signals:
	void detectedBadValue(unsigned long long value) const;

private:
	QWidget*				_parent;
	QLineEdit*				_line;
	QListWidget*			_list;
	std::map<int, QColor>	_lineContainer;
	mutable std::mt19937 mt_rand;

	bool lineParse();
	void insertSingleOrRange(int num, int beginNum);
	void insertValueWithUniqColor(int value);

	QColor generateColor() const;
	QColor generateUniqColor() const;
	
private slots:
	void provideContextMenu(const QPoint &pos);

	void valueError(unsigned long long value);
	void valueChanged();

	void eraseItem();
	void changeItemColor();

	void saveAndClose();
};