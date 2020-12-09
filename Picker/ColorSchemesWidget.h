#pragma once

#include <QDialog>
#include <QDataStream>

#include <vector>
#include <map>
#include <set>
#include <utility>

class QFile;
class QTreeView;
class QColor;
class QStandardItemModel;
class ColorsDelegate;
class QString;
class QItemSelection;

using ColorScheme = std::map<int, QColor>;

class ColorSchemesWidget : public QDialog
{
	Q_OBJECT

public:
	using DataType = std::vector<std::pair<QString, ColorScheme>>;

	explicit ColorSchemesWidget(QFile& file, QStandardItemModel& model,
															QDialog *parent = Q_NULLPTR);
	~ColorSchemesWidget();

	ColorScheme currentScheme();

public slots:
	void save();

signals:
	void dataChanged(bool isChanged) const;
	void isSingleHighlight(bool isSingle) const;

private slots:
	void provideContextMenu(const QPoint &pos);

	void selectKit();
	void clearTable();
	void createKit();

	void erase();
	void copy();
	void editTableLine();

private:	
	QTreeView*			_table;
	QStandardItemModel*	_model;
	QFile*				_appFile;
	QDataStream			_ioFile;
	DataType			_data;
	ColorScheme			_currentScheme;
	std::set<int>		_selectedIndex;

	void choosedLine(const QItemSelection& selected, const QItemSelection& deselected);
	bool isUniqName(const QString & name) const;
	void appendTable();

	void closeEvent(QCloseEvent *event) override;

	int closeMsgBox();
};
