#pragma once

#include <QItemDelegate>

class ColorsDelegate : public QItemDelegate
{
	Q_OBJECT

public:
	enum Roles
	{
		ColorRole = Qt::UserRole,
		NumberRole
	};
	ColorsDelegate(QObject *parent);
	~ColorsDelegate();

	void paint(QPainter *painter, const QStyleOptionViewItem &option,
		const QModelIndex &index) const override;
};
