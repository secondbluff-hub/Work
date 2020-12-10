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
	void detectedBadValue(int value) const;
	void containerChanged(bool isChaged);

private slots:
	void provideContextMenu(const QPoint &pos);

	void valueError(unsigned long long value);
	void valueChanged();

	void erase();
	void changeItemColor();

private:
	QLineEdit*				_line;
	QListWidget*			_list;

	std::map<int, QColor>	_lineContainer;

	bool lineParse();
	void insertSingleOrRange(int num, int beginNum);
	void insertValueWithContrastColor(int value);

	void addListItem(int num, const QColor& color);

	int closeMsgBox();

	void closeEvent(QCloseEvent *event) override;
};