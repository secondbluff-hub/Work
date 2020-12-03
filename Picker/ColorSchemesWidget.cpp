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

	_delegate = new ColorsDelegate(this);

	_table = new QTreeView(this);
	_table->setModel(_model);
	_table->setItemDelegateForColumn(1, _delegate);

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

ColorSchemesWidget::ColorSchemesWidget(const QString& folder, QWidget *parent)
	: QWidget(parent)
{
	
}

ColorSchemesWidget::~ColorSchemesWidget()
{

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

void ColorSchemesWidget::editTable()
{
	
}

void ColorSchemesWidget::createKit()
{
	QString name = QInputDialog::getText(this, "Name creator",
		"Enter a name for the new set:",
		QLineEdit::Normal, "");

	Picker w(this);
	if (w.exec() == QDialog::Accepted)
	{
		_data.push_back(std::make_pair(std::move(name), w.numbersToColors()));
		auto nameItem = new QStandardItem(_data.back().first);
		auto colorsItem = new QStandardItem("ColorScheme");
		_model->appendRow({ nameItem, colorsItem });
	}

	emit dataChanged();
}
