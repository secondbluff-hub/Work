#include "Picker.h"
#include "ColorGenerate.h"

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
#include <QMessageBox>

Picker::Picker(QWidget *parent) : QDialog(parent)
{
	setWindowTitle("Color picker");

	auto _label = new QLabel(tr("Enter numbers"));

	_line = new QLineEdit;
	_line->setStyleSheet("border-style: solid, border-width: 1px; border-color: black");

	auto regExp = new QRegularExpression("(\\d{0,6}+(\\-{1}\\d{0,6}+\\s{1}|\\s{1}))*\\s*");
	auto validator = new QRegularExpressionValidator(*regExp);
	_line->setValidator(validator);

	auto genButton = new QPushButton("Add");
	auto clearButton = new QPushButton("Clear");

	auto hbox = new QHBoxLayout;
	hbox->addWidget(_line);
	hbox->addWidget(genButton);
	hbox->addWidget(clearButton);

	_list = new QListWidget;
	_list->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(_list, &QListWidget::customContextMenuRequested, this, &Picker::provideContextMenu);

	auto vbox = new QVBoxLayout(this);
	vbox->addWidget(_label);
	vbox->addLayout(hbox);
	vbox->addWidget(_list);

	auto okButton = new QPushButton("OK");
	auto lowHBox = new QHBoxLayout;
	lowHBox->addSpacing(200);
	lowHBox->addWidget(okButton);
	vbox->addLayout(lowHBox);

	connect(okButton, &QPushButton::clicked, this, &Picker::accept);

	connect(genButton, &QPushButton::clicked, this, &Picker::editListBox);
	connect(clearButton, &QPushButton::clicked, this, &Picker::clearListBox);

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
	submenu.addAction("Delete item", this, &Picker::eraseItem);
	submenu.addAction("Change color", this, &Picker::changeItemColor);

	submenu.addAction("Print HSV color");	// For debugging

	QAction* rightClickItem = submenu.exec(item);

	// For debugging
	if (rightClickItem && rightClickItem->text().contains("Print HSV color"))
	{
		qDebug() << _list->itemAt(pos)->text() << ": " <<
			_list->itemAt(pos)->backgroundColor().hue() << ' ' <<
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
			insertValueWithContrastColor(i);
		}
	}
	else
	{
		insertValueWithContrastColor(num);
	}
}

void Picker::insertValueWithContrastColor(int v)
{
	static int lastHue = 0;
	_lineContainer.emplace	(	std::make_pair(v,
								ColorGenerate::generateContrastColor(lastHue, _lineContainer))
							);
}

void Picker::valueError(unsigned long long value)
{
	_line->setStyleSheet("border-style: solid; border-width: 2px; border-color: red");

	auto messageError = tr("Bad value: ") + QString::number(value);
	QToolTip::showText(QCursor::pos(), messageError);

	connect(_line, &QLineEdit::textChanged, this, &Picker::valueChanged);
}

void Picker::valueChanged()
{
	_line->setStyleSheet("border-style: solid; border-width: 1px; border-color: black");

	disconnect(_line, &QLineEdit::textChanged, this, &Picker::valueChanged);
}

std::map<int, QColor>&& Picker::numbersToColors()
{
	return std::move(_lineContainer);
}

void Picker::clearListBox()
{
	_list->clear();
	_lineContainer.clear();
}

void Picker::eraseItem()
{
	_lineContainer.erase(_list->takeItem(_list->currentRow())->text().toInt());
}

void Picker::changeItemColor()
{
	auto newColor = QColorDialog::getColor(_list->currentItem()->backgroundColor(), this);

	_list->currentItem()->setBackgroundColor(newColor);
	int num;

	if (ColorGenerate::isContrastColor(newColor, _lineContainer, num))
	{
		QMessageBox msgBox(this);
		msgBox.setWindowTitle("Warning");
		msgBox.setText("New color is not in contrast to item with number " + QString::number(num));
		auto changeButton = new QPushButton("Change color");
		msgBox.addButton(QMessageBox::Save);
		msgBox.addButton(changeButton, QMessageBox::ActionRole);
		msgBox.setDefaultButton(changeButton);
		connect(changeButton, &QPushButton::clicked, this, &Picker::changeItemColor);
		msgBox.exec();
	}

	_lineContainer[_list->currentItem()->text().toInt()] = newColor;
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
