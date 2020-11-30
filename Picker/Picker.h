#pragma once

#include <QtWidgets/QDialog>

#include <map>
#include <unordered_set>
#include <random>

class QListWidget;
class QLineEdit;
class QColor;

struct QColorHash {
	inline std::size_t operator()(const QColor& v) const {
		return v.hue() * 31 + v.saturation() + v.value();
	}
};

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
	using ColorsConIter = typename std::unordered_set<QColor, QColorHash>::iterator;

	QLineEdit*								_line;
	QListWidget*							_list;
	std::map<int, ColorsConIter>			_lineContainer;
	std::unordered_set<QColor, QColorHash>	_colorsContainer;
	std::mt19937 mt_rand;

	bool lineParse();
	void insertSingleOrRange(int num, int beginNum);
	void insertValueWithUniqColor(int value);
	
	inline int generateRandom(int min, int max);

private slots:
	void provideContextMenu(const QPoint &pos);

	void valueError(unsigned long long value);
	void valueChanged();

	void eraseItem();
	void changeItemColor();
};