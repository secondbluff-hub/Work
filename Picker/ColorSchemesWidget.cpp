#include "ColorSchemesWidget.h"
#include "Picker.h"
#include "ColorsDelegate.h"

#include <set>

#include <QApplication>
#include <QString>
#include <QColor>
#include <QFile>
#include <QTreeView>
#include <QStandardItemModel>
#include <QPushButton>
#include <QLayout>
#include <QList>
#include <QMessageBox>
#include <QInputDialog>
#include <QMenu>
#include <QCloseEvent>
#include <QShortcut>
#include <QClipboard>
#include <qDebug>

ColorSchemesWidget::ColorSchemesWidget(QFile& file, QStandardItemModel& model, QDialog *parent)
	: _model(&model), _appFile(&file), _ioFile(_appFile), QDialog(parent)
{
	setWindowTitle("Color schemes widget[*]");

	auto delegate = new ColorsDelegate(this);

	_table = new QTreeView(this);
	_table->setModel(_model);
	_table->setItemDelegateForColumn(1, delegate);
	_table->setEditTriggers(QTreeView::NoEditTriggers);
	_table->setContextMenuPolicy(Qt::CustomContextMenu);
	_table->setRootIsDecorated(false);
	_table->setSelectionMode(QAbstractItemView::ExtendedSelection);

	connect(_table, &QTreeView::customContextMenuRequested, this, &ColorSchemesWidget::provideContextMenu);

	connect(_table, &QTreeView::doubleClicked, this, &ColorSchemesWidget::editTableLine);
	connect(_table->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ColorSchemesWidget::choosedLine);

	auto vbox = new QVBoxLayout(this);
	vbox->addWidget(_table);

	auto selectBtn = new QPushButton("Select");
	selectBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	selectBtn->setEnabled(false);

	connect(this, &ColorSchemesWidget::isSingleHighlight, selectBtn, &QPushButton::setEnabled);

	auto saveBtn = new QPushButton("Save");
	saveBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	auto clearBtn = new QPushButton("Clear");
	clearBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	auto addBtn = new QPushButton("Add");
	addBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	connect(selectBtn, &QPushButton::clicked, this, &ColorSchemesWidget::selectKit);
	connect(saveBtn, &QPushButton::clicked, this, &ColorSchemesWidget::save);
	connect(clearBtn, &QPushButton::clicked, this, &ColorSchemesWidget::clearTable);
	connect(addBtn, &QPushButton::clicked, this, &ColorSchemesWidget::createKit);

	auto hbox = new QHBoxLayout;
	hbox->addWidget(selectBtn);
	hbox->addStretch();
	hbox->addWidget(saveBtn);
	hbox->addWidget(clearBtn);
	hbox->addWidget(addBtn);

	vbox->addLayout(hbox);

	QShortcut *shortcut_copy = new QShortcut(QKeySequence::Copy, _table);
	connect(shortcut_copy, &QShortcut::activated, this, &ColorSchemesWidget::copy);

	QShortcut *shortcut_paste = new QShortcut(QKeySequence::Paste, _table);
	connect(shortcut_paste, &QShortcut::activated, this, &ColorSchemesWidget::paste);

	QShortcut *shortcut_erase = new QShortcut(QKeySequence::Delete, _table);
	connect(shortcut_erase, &QShortcut::activated, this, &ColorSchemesWidget::erase);

	if (_appFile->size() > 0)
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

	connect(this, &ColorSchemesWidget::dataChanged, this, &ColorSchemesWidget::setWindowModified);
}

ColorSchemesWidget::~ColorSchemesWidget()
{
	_appFile->close();
}

void ColorSchemesWidget::clearTable()
{
	_data.clear();

	_model->setRowCount(0);

	emit dataChanged(true);
}

void ColorSchemesWidget::selectKit()
{
	_currentScheme = _data[_table->currentIndex().row()].second;
	
	switch (closeMsgBox()) {
	case QMessageBox::Cancel:
		break;
	case QMessageBox::Save:
		save();
	case QMessageBox::Discard:
		accept();
	}
}

void ColorSchemesWidget::save()
{
	if (isWindowModified())
	{
		_appFile->resize(0);

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

		_appFile->flush();

		emit dataChanged(false);
	}
}

void ColorSchemesWidget::erase()
{
	std::set<int> indexes;

	for (const auto& index : _table->selectionModel()->selection().indexes())
	{
		if (index.column() == 1)
		{
			indexes.insert(index.row());
		}
	}

	for (auto& index = indexes.rbegin(); index != indexes.rend(); ++index)
	{
		_data.erase(_data.begin() + *index);
		_model->removeRow(*index);
	}
	
	emit dataChanged(true);
}

void ColorSchemesWidget::copy()
{
	QString text = QString::number(_table->selectionModel()->selection().indexes().size() / _model->columnCount());

	for (const auto& index : _table->selectionModel()->selection().indexes())
	{
		if (index.column() == 1)
		{
			auto name = _data[index.row()].first;
			
			text += ' ' + name + ' ' + QString::number(_data[index.row()].second.size());

			for (const auto& numToColor : _data[index.row()].second)
			{
				text += ' ' + QString::number(numToColor.first) + ' ' + numToColor.second.name();
			}
		}
	}
	QApplication::clipboard()->setText(text);
}

void ColorSchemesWidget::paste()
{
	if (QApplication::clipboard()->text().size())
	{
		auto listText = QApplication::clipboard()->text().trimmed().split(' ', QString::SkipEmptyParts);
		int size = listText.front().toInt();

		int index = 1;

		for (int i = 0; i < size; ++i)
		{
			auto name = listText[index++];

			while (!isUniqName(name))
			{
				name.append("_1");
			}
			auto groupSize = listText[index++].toInt();

			ColorScheme scheme;
			bool isOdd = true;
			int num;
			for (int j = 0; j < groupSize * 2; ++j)
			{
				if (isOdd)
				{
					num = listText[index++].toInt();
				}
				else
				{
					scheme.emplace(std::make_pair(num, listText[index++]));
				}
				isOdd = !isOdd;
			}
			_data.push_back(std::make_pair(name, scheme));

			appendTable();
		}
	}
}

void ColorSchemesWidget::editTableLine()
{
	if (_table->currentIndex().column() == 0)
	{
		QString name = QInputDialog::getText(this, "Name creator",
			"Enter a new name:",
			QLineEdit::Normal, _data[_table->currentIndex().row()].first);

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

				QList<QVariant> colors;
				QList<QVariant> numbers;

				for (const auto& c : _data[currRow].second)
				{
					numbers += c.first;
					colors += c.second.rgba();
				}

				auto colorsItem = new QStandardItem();
				colorsItem->setData(colors, ColorsDelegate::ColorRole);
				colorsItem->setData(numbers, ColorsDelegate::NumberRole);

				_model->setItem(_table->currentIndex().row(), _table->currentIndex().column(),
																						colorsItem);

				emit dataChanged(true);
			}
		}
	}
}

int ColorSchemesWidget::closeMsgBox()
{
	QMessageBox msgBox;
	msgBox.setText("The document has been modified.");
	msgBox.setInformativeText("Do you want to save your changes?");
	msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Save);
	return msgBox.exec();
}

void ColorSchemesWidget::closeEvent(QCloseEvent *event)
{
	if (isWindowModified())
	{
		switch (closeMsgBox()) {
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
	QList<QVariant> colors;
	QList<QVariant> numbers;
	for (const auto& c : _data.back().second)
	{
		numbers += c.first;
		colors += c.second.rgba();
	}
	auto colorsItem = new QStandardItem();
	colorsItem->setData(colors, ColorsDelegate::ColorRole);
	colorsItem->setData(numbers, ColorsDelegate::NumberRole);
	_model->appendRow({ nameItem, colorsItem });

	emit dataChanged(true);
}

ColorScheme ColorSchemesWidget::currentScheme()
{
	return _currentScheme;
}

void ColorSchemesWidget::choosedLine(const QItemSelection& selected, const QItemSelection& deselected)
{
	if(_table->selectionModel()->selection().size() == 1)
	{
		emit isSingleHighlight(true);
	}
	else
	{
		emit isSingleHighlight(false);
	}
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
	auto delAct = submenu.addAction("Delete", this, &ColorSchemesWidget::erase);
	delAct->setShortcut(QKeySequence::Delete);
	addAction(delAct);
	auto copyAct = submenu.addAction("Copy", this, &ColorSchemesWidget::copy);
	copyAct->setShortcut(QKeySequence::Copy);
	addAction(copyAct);
	auto pasteAct = submenu.addAction("Paste", this, &ColorSchemesWidget::paste);
	pasteAct->setShortcut(QKeySequence::Paste);
	addAction(pasteAct);

	if (_table->selectionModel()->selection().size() == 0)
	{
		delAct->setEnabled(false);
		copyAct->setEnabled(false);
	}

	if (QApplication::clipboard()->text().size() == 0)
	{
		pasteAct->setEnabled(false);
	}

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
