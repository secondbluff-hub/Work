#include "Picker.h"

#include <QRegularExpressionValidator>
#include <QRegularExpression>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QLayout>
#include <Qstring>
#include <QDebug>
#include <QMenu>
#include <QToolTip>
#include <QCursor>
#include <QColor>

#include <utility>
#include <algorithm>

Picker::Picker(QWidget *parent) : QDialog(parent)
{
	auto _label = new QLabel(tr("Enter numbers"));

	_line = new QLineEdit;
	_line->setStyleSheet("border-style: solid, border-width: 1px; border-color: black");

	auto regExp = new QRegularExpression("^[\\d\\s|-]+$");
	auto validator = new QRegularExpressionValidator(*regExp);
	_line->setValidator(validator);

	_genButton = new QPushButton("Add");
	_clearButton = new QPushButton("Clear");

	auto hbox = new QHBoxLayout;
	hbox->addWidget(_line);
	hbox->addWidget(_genButton);
	hbox->addWidget(_clearButton);

	_list = new QListWidget;
	_list->setAlternatingRowColors(true);
	_list->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(_list, &QListWidget::customContextMenuRequested, this, &Picker::provideContextMenu);

	_vbox = new QVBoxLayout(this);
	_vbox->addWidget(_label);
	_vbox->addLayout(hbox);
	_vbox->addWidget(_list);

	connect(_genButton, &QPushButton::clicked, this, &Picker::editListBox);
	connect(_clearButton, &QPushButton::clicked, this, &Picker::clearListBox);

	connect(this, &Picker::detectedBadValue, this, &Picker::valueError);
}

void Picker::editListBox()
{
	_list->clear();

	if(lineParse())
	{
		int i = 0;
		for (const auto& n : _lineContainer)
		{
			_list->addItem(QString::number(n.first));
			_list->item(i++)->setBackgroundColor(n.second);
		}

		_line->clear();
	}
}

void Picker::provideContextMenu(const QPoint &pos)
{
	QPoint item = _list->mapToGlobal(pos);
	QMenu submenu;
	submenu.addAction("Delete");
	QAction* rightClickItem = submenu.exec(item);
	if (rightClickItem && rightClickItem->text().contains("Delete"))
	{
		_lineContainer.erase(_list->takeItem(_list->indexAt(pos).row())->text().toInt());
	}
}

void Picker::insertSingleOrRange(int num, int beginNum = -1)
{
	if (beginNum >= 0) {
		int increment = beginNum < num ? 1 : -1;
		for (int i = beginNum; i != num + increment; i += increment)
		{
			insertValueWithRandomColor(i);
		}
	}
	else
	{
		insertValueWithRandomColor(num);
	}
}

void Picker::insertValueWithRandomColor(int value)
{
	/*
	auto rnd = rand() % 2;
	auto color = QColor	{	rnd >= 0	&&	--rnd < 0	?	0	:	rand() % 255,
							rnd >= 0	&&	--rnd < 0	?	0	:	rand() % 255,
							rnd >= 0	&&	--rnd < 0	?	0	:	rand() % 255,
							200
						};
	*/
	int minRange = 81;

	int hue = rand() % 255;
	hue = hue < 200 && hue > 150 ? (hue + minRange) % 255 : hue;
	int saturation = rand() % 105 + 150;
	int brightness = 210;

	auto it = _lineContainer.emplace(std::make_pair	(value, QColor::fromHsv(hue, saturation, brightness)));
	if (it.second)
	{

		if (_lineContainer.size() != 1)
		{
			auto copyIt = it.first;

			int hue = 0;
			int minRange = 100;

			if (it.first != _lineContainer.begin() && *it.first != *_lineContainer.rbegin())
			{
				auto nextColor = (++copyIt)->second;
				--copyIt;
				auto prevColor = (--copyIt)->second;

				if(abs(prevColor.hslHue() - nextColor.hslHue()) < minRange)
				{
					hue = (std::max(prevColor.hslHue(), nextColor.hslHue()) + minRange) % 255;
				}
			}
			else if (it.first == _lineContainer.begin())
			{
				hue = ((++copyIt)->second.hslHue() + minRange) % 255;
			}
			else
			{
				hue = ((--copyIt)->second.hslHue() + minRange) % 255;
			}
			/*
			if (it.first != _lineContainer.begin() && *it.first != *_lineContainer.rbegin())
			{
				auto oldColor1 = (++copyIt)->second;
				--copyIt;
				auto oldColor2 = (--copyIt)->second;
				color = {	oldColor1.red()		*	oldColor2.red(),
							oldColor1.green()	*	oldColor2.green(),
							oldColor1.blue()	*	oldColor2.blue()
						};
			}
			else if (it.first == _lineContainer.begin())
			{
				color = (++copyIt)->second;
			}
			else
			{
				color = (--copyIt)->second;
			}

			rnd = rand() % 2;
			QColor newColor	{	(color.red() != 0	&&	rnd >= 0 && --rnd < 0) || color.red() == 0		?	rand() % 195 + 30	:	0,
								(color.green() != 0 &&	rnd >= 0 && --rnd < 0) || color.green() == 0	?	rand() % 195 + 30	:	0,
								(color.blue() != 0	&&	rnd >= 0 && --rnd < 0) || color.blue() == 0		?	rand() % 195 + 30	:	0,
								200
							};
			//it.first->second = newColor.lighter(120).convertTo(QColor::Hsv);
			*/
			hue = hue < 200 && hue > 150 ? (hue + minRange) % 255 : hue;
			it.first->second = QColor::fromHsv(hue, saturation, brightness);
		}
	}

	_colorsContainer.emplace(QColor::fromHsv(hue, saturation, brightness));
}

void Picker::valueError(long long value)
{
	_line->setStyleSheet("border-style: solid; border-width: 2px; border-color: red");

	auto messageError = tr("Bad value: ") + QString::number(value);
	QToolTip::showText(QCursor::pos(), messageError);

	connect(_line, &QLineEdit::textChanged, this, &Picker::valueRight);
}

void Picker::valueRight()
{
	_line->setStyleSheet("border-style: solid; border-width: 1px; border-color: black");

	disconnect(_line, &QLineEdit::textChanged, this, &Picker::valueRight);
}

void Picker::clearListBox()
{
	_list->clear();
	_lineContainer.clear();
}

bool Picker::lineParse()
{
	QString number;

	auto nums = _line->text().trimmed().split(' ', QString::SkipEmptyParts);
	for (const auto& n : nums)
	{
		auto num = n.split('-', QString::SkipEmptyParts);

		for (const auto& nn : num)
		{
			if (nn.toInt() > 0xffff)
			{
				emit detectedBadValue(nn.toInt());
				return false;
			}
		}

		if (num.size() == 1)
		{
			insertSingleOrRange(num.at(0).toInt());
		}
		else
		{
			insertSingleOrRange(num.at(1).toInt(), num.at(0).toInt());
		}
	}

	return true;
}