#pragma once

#include <QItemDelegate>

class ColorsDelegate : public QItemDelegate
{
	Q_OBJECT

public:
	ColorsDelegate(QObject *parent);
	~ColorsDelegate();

	void paint(QPainter *painter, const QStyleOptionViewItem &option,
		const QModelIndex &index) const override;
};
