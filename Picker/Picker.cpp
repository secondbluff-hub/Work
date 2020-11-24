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

Picker::Picker(QWidget *parent) : QDialog(parent)
{
	auto _label = new QLabel(tr("Enter numbers"));

	_line = new QLineEdit;

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
}

void Picker::editListBox()
{
	lineParse();

	_list->clear();

	for (const auto& n : _lineContainer)
	{
		_list->addItem(QString::number(n));
	}
	_line->clear();
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
	if (num > 0xffff) {
		return;
	}

	if (beginNum >= 0) {
		int increment = beginNum < num ? 1 : -1;
		for (int i = beginNum; i != num + increment; i += increment) {
			_lineContainer.insert(i);
		}
	}
	else
	{
		_lineContainer.insert(num);
	}
}

void Picker::clearListBox()
{
	_list->clear();
	_lineContainer.clear();
}

void Picker::lineParse()
{
	QString number;

	auto nums = _line->text().trimmed().split(' ', QString::SkipEmptyParts);
	for (const auto& n : nums)
	{
		auto num = n.split('-', QString::SkipEmptyParts);
		if (num.size() == 1)
		{
			insertSingleOrRange(num.at(0).toInt());
		}
		else
		{
			insertSingleOrRange(num.at(1).toInt(), num.at(0).toInt());
		}
	}
}
