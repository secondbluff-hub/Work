#include "ColorSchemesWidget.h"
#include "Picker.h"
#include "ColorsDelegate.h"

#include <QString>
#include <QColor>
#include <QTreeView>
#include <QStandardItemModel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QList>
#include <QMessageBox>
#include <QInputDialog>
#include <QMenu>
#include <QCloseEvent>
#include <qDebug>

ColorSchemesWidget::ColorSchemesWidget(const QString& file, QWidget *parent)
	: QWidget(parent)
{
	setWindowTitle("Color schemes widget[*]");

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
	selectBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	auto saveBtn = new QPushButton("Save");
	saveBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	auto clearBtn = new QPushButton("Clear");
	clearBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	auto addBtn = new QPushButton("Add");
	addBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	auto spacer = new QSpacerItem(200, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

	connect(selectBtn, &QPushButton::clicked, this, &ColorSchemesWidget::selectKit);
	connect(saveBtn, &QPushButton::clicked, this, &ColorSchemesWidget::save);
	connect(clearBtn, &QPushButton::clicked, this, &ColorSchemesWidget::clearTable);
	connect(addBtn, &QPushButton::clicked, this, &ColorSchemesWidget::createKit);

	auto hbox = new QHBoxLayout;
	hbox->addWidget(selectBtn);
	hbox->addSpacerItem(spacer);
	hbox->addWidget(saveBtn);
	hbox->addWidget(clearBtn);
	hbox->addWidget(addBtn);

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
		}
	}

	connect(this, &ColorSchemesWidget::dataChanged, this, &ColorSchemesWidget::setWindowModified);
}

ColorSchemesWidget::~ColorSchemesWidget()
{
	_appFile.close();
}

void ColorSchemesWidget::clearTable()
{
	_data.clear();

	_model->setRowCount(0);

	emit dataChanged(true);
}

void ColorSchemesWidget::selectKit() const
{
	qDebug() << "Call select";
}

void ColorSchemesWidget::save()
{
	if (isWindowModified())
	{
		_appFile.resize(0);

		_ioFile.setVersion(QDataStream::Qt_5_9);
		if (_data.size() > 0)
		{
			_ioFile << static_cast<qint32>(_data.size());

			for (int i = 0; i < _data.size(); ++i)
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

		emit dataChanged(false);
	}
}

void ColorSchemesWidget::eraseLine()
{
	int currRow = _table->currentIndex().row();
	_data.erase(_data.begin() + currRow);
	_model->removeRow(currRow);
}

void ColorSchemesWidget::copyLine()
{
	auto name = _data[_table->currentIndex().row()].first;
	while (!isUniqName(name))
	{
		name.append("_1");
	}

	_data.push_back(std::make_pair(name, _data[_table->currentIndex().row()].second));

	appendTable();
}

void ColorSchemesWidget::editTableLine()
{
	if (_table->currentIndex().column() == 0)
	{
		QString name = QInputDialog::getText(this, "Name creator",
			"Enter a new name for " + _data[_table->currentIndex().row()].first + ":",
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

			emit dataChanged(true);
		}
	}
	else if (_table->currentIndex().column() == 1)
	{
		Picker w(_data[_table->currentIndex().row()].second, this);

		if (w.exec() == QDialog::Accepted)
		{
			if (w.isWindowModified())
			{
				int currRow = _table->currentIndex().row();
				_data[currRow].second = w.numbersToColors();

				QString colors;
				for (const auto& c : _data[currRow].second)
				{
					colors += QString::number(c.first) + ' ' + c.second.name() + ' ';
				}

				_model->setData(_table->currentIndex(), colors);

				emit dataChanged(true);
			}
		}
	}
}

void ColorSchemesWidget::closeEvent(QCloseEvent *event)
{
	if (isWindowModified())
	{
		QMessageBox msgBox;
		msgBox.setText("The document has been modified.");
		msgBox.setInformativeText("Do you want to save your changes?");
		msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Save);
		int ret = msgBox.exec();
		
		switch (ret) {
		case QMessageBox::Cancel:
			event->ignore();
			break;
		case QMessageBox::Save:
			save();
		case QMessageBox::Discard:
			event->accept();
		}
	}
	else
	{
		event->accept();
	}
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

	emit dataChanged(true);
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
}
