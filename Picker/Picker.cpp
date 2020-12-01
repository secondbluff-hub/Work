#include "Picker.h"
#include "ColorSchemesWidget.h"

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

template <typename T>
int generateRandom(T& mt_rand, int min, int max)
{
	std::uniform_int_distribution<int> range(min, max);
	return range(mt_rand);
}


Picker::Picker(QWidget *parent) : QDialog(parent)
{
	_parent = parent;

	this->setWindowTitle("Color picker");

	std::random_device device;
	mt_rand.seed(device());

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

	connect(okButton, &QPushButton::clicked, this, &Picker::saveAndClose);

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
	QColor color = generateColor();
	static int lastHue;

	if (_lineContainer.size() <= 30 && _lineContainer.size() > 0)
	{
		int hue = color.hue();
		int step = 144;
		hue = (lastHue + step);
		if ((_lineContainer.size() + 1) % 5 == 0)
		{
			hue += 12;
		}
		hue %= 360;
		color.setHsv(hue, color.saturation(), color.value());
	}
	else
	{
		color = generateUniqColor();
	}

	lastHue = color.hue();
	_lineContainer.emplace(std::make_pair(v,color));
}

QColor Picker::generateColor() const
{
	return QColor::fromHsv(	generateRandom(mt_rand, 0, 359),
							generateRandom(mt_rand, 200, 255),
							generateRandom(mt_rand, 200, 255));
}

QColor Picker::generateUniqColor() const
{
	//static int minRange = 75;
	//static int maxRange = 360 - minRange / 2;
	auto isUniqColor = [&_lineContainer = _lineContainer](const QColor& color)
	{
		for (const auto& numToColor : _lineContainer)
		{
			if (numToColor.second == color) {
				return false;
			}
		}
		return true;
	};

	QColor color;

	do
	{
		color = generateColor();
	} while (!isUniqColor(color));

	return color;
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

std::map<int, QColor> Picker::numbersToColors() const
{
	return _lineContainer;
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

	int minRange = 360 / (_lineContainer.size() + 1);
	int maxRange = 360 - minRange / 2;
	bool isContrast = true;
	int num;
	for (const auto numToColor : _lineContainer)
	{
		if (	abs(newColor.hue() - numToColor.second.hue()) < minRange ||
				abs(newColor.hue() - numToColor.second.hue()) > maxRange	)
		{
			num = numToColor.first;
			isContrast = false;
			break;
		}
	}

	if (!isContrast)
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

void Picker::saveAndClose()
{
	dynamic_cast<ColorSchemesWidget*>(_parent)->insertData(std::move(_lineContainer));
	this->close();
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