#include "ColorSchemesWidget.h"
#include "Picker.h"

#include <QVBoxLayout>
#include <QTreeView>
#include <QPushButton>
#include <QStandardItemModel>
#include <QList>
#include <QColor>
#include <qDebug>

ColorSchemesWidget::ColorSchemesWidget(QWidget *parent)
	: QWidget(parent)
{
	this->setWindowTitle("ColorSchemesWidget[*]");

	auto model = new QStandardItemModel();
	model->setColumnCount(2);
	model->setHeaderData(0, Qt::Orientation::Horizontal, "Name");
	model->setHeaderData(1, Qt::Orientation::Horizontal, "Colors");

	_table = new QTreeView(this);
	_table->setModel(model);

	auto vbox = new QVBoxLayout(this);
	vbox->addWidget(_table);

	auto selectBtn = new QPushButton("Select");
	auto clearBtn = new QPushButton("Clear");
	auto createBtn = new QPushButton("Create");

	connect(createBtn, &QPushButton::clicked, this, &ColorSchemesWidget::createKit);
	connect(clearBtn, &QPushButton::clicked, this, &ColorSchemesWidget::clearTable);
	connect(selectBtn, &QPushButton::clicked, this, &ColorSchemesWidget::selectKit);

	connect(this, &ColorSchemesWidget::dataChanged, this, &ColorSchemesWidget::setWindowModified);
	connect(this, &ColorSchemesWidget::dataChanged, this, &ColorSchemesWidget::editTable);

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
	auto w = new Picker(this);
	w->show();
}

void ColorSchemesWidget::insertData(std::map<int, QColor>&& newData)
{
	auto oldSize = _data.size();
	_data.insert(make_move_iterator(newData.begin()), make_move_iterator(newData.end()));
	if(oldSize - _data.size() > 0)
	{
		emit dataChanged();
		disconnect(this, &ColorSchemesWidget::dataChanged, this, &ColorSchemesWidget::setWindowModified);
	}

	qDebug() << _data.size();
}