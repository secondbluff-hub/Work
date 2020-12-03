#include "ColorsDelegate.h"

#include <QPainter>
#include <QStyleOptionViewItem>
#include <QModelIndex>

ColorsDelegate::ColorsDelegate(QObject *parent)
	: QItemDelegate(parent)
{

}

ColorsDelegate::~ColorsDelegate()
{

}

void ColorsDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option,
	const QModelIndex & index) const
{
	if (index.isValid() && index.column() == 1)
	{
		painter->save();

		painter->setPen(Qt::red);		
		painter->drawRect(option.rect);
		painter->drawText(option.rect, index.data(Qt::DisplayRole).toString());

		painter->restore();
	}
}

