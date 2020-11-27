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
#include <QColorDialog>

#include <utility>
#include <algorithm>

Picker::Picker(QWidget *parent) : QDialog(parent)
{
	std::random_device device;
	mt_rand.seed(device());

	auto _label = new QLabel(tr("Enter numbers"));

	_line = new QLineEdit;
	_line->setStyleSheet("border-style: solid, border-width: 1px; border-color: black");

	auto regExp = new QRegularExpression("^[\\d*(\\s{1})|(-{1})]+$");
	//auto regExp = new QRegularExpression("(\\d+(\\s{1})+)|(\\d+\\-{1}\\d+)*\\s*"); // В процессе
	auto validator = new QRegularExpressionValidator(*regExp);
	_line->setValidator(validator);

	_genButton = new QPushButton("Add");
	_clearButton = new QPushButton("Clear");

	auto hbox = new QHBoxLayout;
	hbox->addWidget(_line);
	hbox->addWidget(_genButton);
	hbox->addWidget(_clearButton);

	_list = new QListWidget;
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
			_list->item(i++)->setBackgroundColor(*n.second);
		}

		_line->clear();
	}
}

void Picker::provideContextMenu(const QPoint &pos)
{
	QPoint item = _list->mapToGlobal(pos);
	QMenu submenu;
	submenu.addAction("Delete item", this, &Picker::eraseItem);
	submenu.addAction("Change color", this, &Picker::changeItemColor);

	submenu.addAction("Print HSV color");	// Для отладки

	QAction* rightClickItem = submenu.exec(item);

	// Для отладки
	if (rightClickItem && rightClickItem->text().contains("Print HSV color"))
	{
		qDebug() << _list->itemAt(pos)->backgroundColor().hue() << ' ' <<
			_list->itemAt(pos)->backgroundColor().saturation() << ' ' <<
			_list->itemAt(pos)->backgroundColor().value();
	}
}

void Picker::insertSingleOrRange(int num, int beginNum = -1)
{
	if (beginNum >= 0) {
		int increment = beginNum < num ? 1 : -1;
		for (int i = beginNum; i != num + increment; i += increment)
		{
			insertValueWithUniqColor(i);
		}
	}
	else
	{
		insertValueWithUniqColor(num);
	}
}

void Picker::insertValueWithUniqColor(int v)
{
	int hue, saturation, value;
	generateColor(hue, saturation, value);

	if (_lineContainer.size() != 0)
	{
		generateUniqColor(hue, saturation, value);
	}

	auto it = _lineContainer.emplace(std::make_pair	(v, _colorsContainer.emplace(QColor::fromHsv(hue, saturation, value)).first));
	if (it.second)
	{

		if (_lineContainer.size() > 1)
		{
			do
			{
				int minRange = generateRandom(81, 131); // rand() % 50 + 81;
				auto copyIt = it.first;

				if (it.first != _lineContainer.begin() && *it.first != *_lineContainer.rbegin())
				{
					auto nextColorHue = (++copyIt)->second->hsvHue();
					--copyIt;
					auto prevColorHue = (--copyIt)->second->hsvHue();
					auto colorsDistance = abs(prevColorHue - nextColorHue);

					if (colorsDistance < minRange * 2)
					{
						hue = (std::max(prevColorHue, nextColorHue) + minRange) % 255;
					}
					else
					{
						hue = colorsDistance / 2;
					}
				}
				else if (it.first == _lineContainer.begin())
				{
					hue = ((++copyIt)->second->hsvHue() + minRange) % 255;
				}
				else
				{
					hue = ((--copyIt)->second->hsvHue() + minRange) % 255;
				}
			} while (_colorsContainer.find(QColor::fromHsv(hue, saturation, value)) != _colorsContainer.end());
			_colorsContainer.erase(*it.first->second);
			it.first->second = _colorsContainer.emplace(QColor::fromHsv(hue, saturation, value)).first;
		}
	}

}

void Picker::generateColor(int & hue, int & saturation, int & value)
{
	int minRange = generateRandom(51, 101); // rand() % 50 + 51;
	hue = generateRandom(1, 255); // rand() % 255;
	saturation = generateRandom(200, 255); // rand() % 55 + 200;
	value = generateRandom(200, 255); // rand() % 55 + 200;
}

void Picker::generateUniqColor(int & hue, int & saturation, int & value)
{
	do
	{
		generateColor(hue, saturation, value);
	} while (_colorsContainer.find(QColor::fromHsv(hue, saturation, value)) != _colorsContainer.end());
}

int Picker::generateRandom(int min, int max)
{
	std::uniform_int_distribution<int> range(min, max);
	return range(mt_rand);
}

void Picker::valueError(unsigned long long value)
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
	_colorsContainer.clear();
}

void Picker::eraseItem()
{
	_colorsContainer.erase(_list->currentItem()->backgroundColor());
	_lineContainer.erase(_list->takeItem(_list->currentRow())->text().toInt());
}

void Picker::changeItemColor()
{
	auto newColor = QColorDialog::getColor(Qt::white, this);

	_colorsContainer.erase(_list->currentItem()->backgroundColor());

	_list->currentItem()->setBackgroundColor(newColor);
	_lineContainer[_list->currentItem()->text().toInt()] = _colorsContainer.insert(newColor).first;
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
			auto currNum = nn.toULongLong();
			if (currNum > 0xffff || currNum == 0)
			{
				emit detectedBadValue(currNum);
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