#include "ColorsDelegate.h"

#include <QPainter>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QPen>
#include <QDebug>

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

		auto numAndColors = index.data(Qt::DisplayRole).toString().split(' ', QString::SkipEmptyParts);

		int x = option.rect.x();
		int y = option.rect.y();
		//int w = option.rect.width()/10;
		int w = 20;
		int h = option.rect.height();

		int yPoint = h * (index.row() + 1) - 4;

		bool isEven = false;
		for (const auto& item : numAndColors)
		{
			if (isEven)
			{
				if (option.rect.width() - x + option.rect.x() < 2 * w)
				{
					QPoint point1(x + (option.rect.x() + option.rect.width() - x) / 5 * 2, yPoint);
					QPoint point2(x + (option.rect.x() + option.rect.width() - x) / 5 * 3, yPoint);
					QPoint point3(x + (option.rect.x() + option.rect.width() - x) / 5 * 4, yPoint);

					QPen pen(Qt::black);
					pen.setWidth(2);
					painter->setPen(pen);

					painter->drawPoint(point1);
					painter->drawPoint(point2);
					painter->drawPoint(point3);
				}
				else
				{
					painter->fillRect(x, y + 1, w, h - 2, item);
					x += w + 2;
				}
			}
			isEven = !isEven;
		}

		//painter->setPen(Qt::red);
		//painter->drawRect(option.rect);
		//painter->drawText(option.rect, index.data(Qt::DisplayRole).toString());

		painter->restore();
	}
}

