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
#include <QMenu>
#include <qDebug>

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
	_table->setEditTriggers(QTreeView::NoEditTriggers);
	_table->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(_table, &QTreeView::customContextMenuRequested, this, &ColorSchemesWidget::provideContextMenu);

	connect(_table, &QTreeView::doubleClicked, this, &ColorSchemesWidget::editTableLine);

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
	if (!_appFile.open(QIODevice::ReadWrite))
	{
		qDebug() << "Couldn't open: " << file;
	}
	else
	{
		_ioFile.setDevice(&_appFile);

		if (_appFile.size() > 0)
		{
			if (_ioFile.version() != QDataStream::Qt_5_9)
			{
				qDebug() << "Wrong version of Qt";
			}

			int i;
			_ioFile >> i;

			for (; i > 0; --i)
			{
				QString name;
				_ioFile >> name;

				int j;
				_ioFile >> j;

				std::map<int, QColor> dataColors;
				for (; j > 0; --j)
				{
					int number;
					QString colorName;
					_ioFile >> number >> colorName;

					QColor color(colorName);
					dataColors.insert(std::make_pair(number, color));
				}

				_data.push_back(std::make_pair(name, dataColors));
				appendTable();
			}
			_appFile.resize(0);
		}
	}
}

ColorSchemesWidget::~ColorSchemesWidget()
{
	_ioFile.setVersion(QDataStream::Qt_5_9);
	if (_data.size() > 0)
	{
		_ioFile << static_cast<qint32>(_data.size());

		for (int i = _data.size() - 1; i >= 0; --i)
		{
			_ioFile << _data[i].first;
			_ioFile << static_cast<qint32>(_data[i].second.size());

			for (const auto& numToColor : _data[i].second)
			{
				_ioFile << static_cast<qint32>(numToColor.first);
				_ioFile << numToColor.second.name();
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

void ColorSchemesWidget::selectKit() const
{
	qDebug() << "Call select";
}

void ColorSchemesWidget::eraseLine()
{
	int currRow = _table->currentIndex().row();
	_data.erase(_data.begin() + currRow);
	_model->removeRow(currRow);
}

void ColorSchemesWidget::copyLine()
{
	qDebug() << _model->data(_table->currentIndex());
	//_data.push_back(std::make_pair());
}

void ColorSchemesWidget::editTableLine()
{
	if (_table->currentIndex().column() == 0)
	{
		qDebug() << "EDIT TABLE LINE";

		QString name = QInputDialog::getText(this, "Name creator",
			"Enter a name for the new set:",
			QLineEdit::Normal, "");

		if (name != "")
		{
			while (!isUniqName(name))
			{
				name.append("_1");
			}

			int currRow = _table->currentIndex().row();
			_data[currRow].first = name;
			_model->setData(_table->currentIndex(), name);
		}
	}
	else if (_table->currentIndex().column() == 1)
	{
		Picker w(_data[_table->currentIndex().row()].second, this);

		if (w.exec() == QDialog::Accepted)
		{
			int currRow = _table->currentIndex().row();
			_data[currRow].second = w.numbersToColors();
			/////////////////////////////////////////////////////////////////////
			// Fix: colors are not updated
			/////////////////////////////////////////////////////////////////////
		}
	}

	emit dataChanged();
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

bool ColorSchemesWidget::isUniqName(const QString & name) const
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

void ColorSchemesWidget::provideContextMenu(const QPoint &pos)
{
	QPoint item = _table->mapToGlobal(pos);
	QMenu submenu;
	submenu.addAction("Delete line", this, &ColorSchemesWidget::eraseLine);
	submenu.addAction("Copy line", this, &ColorSchemesWidget::copyLine);

	QAction* rightClickItem = submenu.exec(item);
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
