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

#include <utility>

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
			_list->item(i++)->setForeground(n.second);
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
		_list->takeItem(_list->indexAt(pos).row());
	}
}

void Picker::insertSingleOrRange(int num, int beginNum = -1) {
	if (beginNum >= 0) {
		int increment = beginNum < num ? 1 : -1;
		for (int i = beginNum; i != num + increment; i += increment) {
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
	_lineContainer.insert(std::make_pair(value, static_cast<Qt::GlobalColor>(rand() % 19)));
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
