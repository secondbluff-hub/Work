#pragma once

#include <QtWidgets/QDialog>

#include <map>
#include <unordered_set>

class QListWidget;
class QLineEdit;
class QPushButton;
class QVBoxLayout;
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

public slots:
	void editListBox();
	void clearListBox();

signals:
	void detectedBadValue(long long value);

private:
	QLineEdit*								_line;
	QPushButton*							_genButton;
	QPushButton*							_clearButton;
	QListWidget*							_list;
	QVBoxLayout*							_vbox;
	std::map<int, QColor>					_lineContainer;
	std::unordered_set<QColor, QColorHash>	_colorsContainer;

	bool lineParse();
	void insertSingleOrRange(int num, int beginNum);
	void insertValueWithRandomColor(int value);

private slots:
	void provideContextMenu(const QPoint &pos);
	void valueError(long long value);
	void valueRight();
};