#pragma once

#include <QWidget>
#include <QFile>
#include <QDataStream>

#include <vector>
#include <map>
#include <utility>

class QTreeView;
class QColor;
class QStandardItemModel;
class ColorsDelegate;
class QString;

class ColorSchemesWidget : public QWidget
{
	Q_OBJECT

public:
	using DataType = std::vector<std::pair<QString, std::map<int, QColor>>>;

	explicit ColorSchemesWidget(const QString& folder, QWidget *parent = Q_NULLPTR);
	~ColorSchemesWidget();

public slots:
	void save();

signals:
	void dataChanged(bool isChanged) const;

private:
	QTreeView*			_table;
	QStandardItemModel*	_model;
	QFile				_appFile;
	QDataStream			_ioFile;
	DataType			_data;

	bool isUniqName(const QString & name) const;
	void appendTable();

	void closeEvent(QCloseEvent *event) override;

private slots:
	void provideContextMenu(const QPoint &pos);

	void selectKit() const;
	void clearTable();
	void createKit();

	void eraseLine();
	void copyLine();
	void editTableLine();
};
