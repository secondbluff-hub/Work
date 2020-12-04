#include "ColorSchemesWidget.h"
#include "Picker.h"
#include "ColorsDelegate.h"

#include <QVBoxLayout>
#include <QTreeView>
#include <QPushButton>
#include <QStandardItemModel>
#include <QList>
#include <QColor>
#include <QInputDialog>
#include <QString>
#include <qDebug>

ColorSchemesWidget::ColorSchemesWidget(QWidget *parent)
	: QWidget(parent)
{
	setWindowTitle("ColorSchemesWidget[*]");

	_model = new QStandardItemModel(this);
	_model->setColumnCount(2);
	_model->setHeaderData(0, Qt::Orientation::Horizontal, "Name");
	_model->setHeaderData(1, Qt::Orientation::Horizontal, "Colors");

	auto delegate = new ColorsDelegate(this);

	_table = new QTreeView(this);
	_table->setModel(_model);
	_table->setItemDelegateForColumn(1, delegate);

	auto vbox = new QVBoxLayout(this);
	vbox->addWidget(_table);

	auto selectBtn = new QPushButton("Select");
	auto clearBtn = new QPushButton("Clear");
	auto createBtn = new QPushButton("Create");

	connect(createBtn, &QPushButton::clicked, this, &ColorSchemesWidget::createKit);
	connect(clearBtn, &QPushButton::clicked, this, &ColorSchemesWidget::clearTable);
	connect(selectBtn, &QPushButton::clicked, this, &ColorSchemesWidget::selectKit);

	connect(this, &ColorSchemesWidget::dataChanged, this, &ColorSchemesWidget::setWindowModified);

	auto hbox = new QHBoxLayout;
	hbox->addSpacing(200);
	hbox->addWidget(createBtn);
	hbox->addWidget(clearBtn);
	hbox->addWidget(selectBtn);

	vbox->addLayout(hbox);
}

ColorSchemesWidget::ColorSchemesWidget(const QString& file, QWidget *parent)
	: QWidget(parent)
{
	setWindowTitle("ColorSchemesWidget[*]");

	_model = new QStandardItemModel(this);
	_model->setColumnCount(2);
	_model->setHeaderData(0, Qt::Orientation::Horizontal, "Name");
	_model->setHeaderData(1, Qt::Orientation::Horizontal, "Colors");

	auto delegate = new ColorsDelegate(this);

	_table = new QTreeView(this);
	_table->setModel(_model);
	_table->setItemDelegateForColumn(1, delegate);

	auto vbox = new QVBoxLayout(this);
	vbox->addWidget(_table);

	auto selectBtn = new QPushButton("Select");
	auto clearBtn = new QPushButton("Clear");
	auto createBtn = new QPushButton("Create");

	connect(createBtn, &QPushButton::clicked, this, &ColorSchemesWidget::createKit);
	connect(clearBtn, &QPushButton::clicked, this, &ColorSchemesWidget::clearTable);
	connect(selectBtn, &QPushButton::clicked, this, &ColorSchemesWidget::selectKit);

	connect(this, &ColorSchemesWidget::dataChanged, this, &ColorSchemesWidget::setWindowModified);

	auto hbox = new QHBoxLayout;
	hbox->addSpacing(200);
	hbox->addWidget(createBtn);
	hbox->addWidget(clearBtn);
	hbox->addWidget(selectBtn);

	vbox->addLayout(hbox);

	_appFile.setFileName(file);
	_appFile.open(QFile::ReadWrite);

	_ioFile.setDevice(&_appFile);

	if (_appFile.size() > 0)
	{
		_ioFile.setVersion(QDataStream::Qt_5_9);
		int i;
		_ioFile >> i;
		qDebug() << "i: " << i;
		for (; i > 0; --i)
		{
			QString name;
			_ioFile >> name;
			qDebug() << "name: " << i;
			int j;
			_ioFile >> j;
			qDebug() << "j: " << i;
			std::map<int, QColor> dataColors;
			for (; j > 0; --j)
			{
				int number;
				QColor color;
				_ioFile >> number >> color;
				dataColors.insert(std::make_pair(number, color));
			}
			_data.push_back(std::make_pair(name, dataColors));
			appendTable();
		}
	}
}

ColorSchemesWidget::~ColorSchemesWidget()
{
	_ioFile.setVersion(QDataStream::Qt_5_9);
	if (_data.size() > 0)
	{
		_ioFile << _data.size();
		qDebug() << "_data.size(): " << _data.size();
		for (int i = _data.size() - 1; i >= 0; --i)
		{
			_ioFile << _data[i].first;
			_ioFile << _data[i].second.size();
			for (const auto& numToColor : _data[i].second)
			{
				_ioFile << numToColor.first;
				_ioFile << numToColor.second;
			}
		}
	}
	_appFile.flush();
	_appFile.close();
}

void ColorSchemesWidget::clearTable()
{
	_data.clear();

	_model->setRowCount(0);

	emit dataChanged();
}

void ColorSchemesWidget::selectKit()
{
	qDebug() << "Call select";
}

void ColorSchemesWidget::appendTable()
{
	auto nameItem = new QStandardItem(_data.back().first);
	QString colors;
	for (const auto& c : _data.back().second)
	{
		colors += QString::number(c.first) + ' ' + c.second.name() + ' ';
	}
	auto colorsItem = new QStandardItem(std::move(colors));
	_model->appendRow({ nameItem, colorsItem });
}

void ColorSchemesWidget::updateFile()
{

}

bool ColorSchemesWidget::isUniqName(const QString & name)
{
	for (const auto& n : _data)
	{
		if (n.first == name)
		{
			return false;
		}
	}
	return true;
}

void ColorSchemesWidget::createKit()
{
	QString name = QInputDialog::getText(this, "Name creator",
		"Enter a name for the new set:",
		QLineEdit::Normal, "");

	while (!isUniqName(name))
	{
		name.append("_1");
	}

	Picker w(this);
	if (w.exec() == QDialog::Accepted)
	{
		_data.push_back(std::make_pair(std::move(name), w.numbersToColors()));
		appendTable();
	}

	emit dataChanged();
}
