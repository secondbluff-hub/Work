#pragma once

#include <QDialog>
#include <QDataStream>

#include <vector>
#include <map>
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

	ColorSchemesWidget(const DataType& data,
						QStandardItemModel& model, 
						QWidget* parent = nullptr);

	ColorScheme currentScheme();
	DataType forSaving();

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
	void paste();
	void editTableLine();

private:	
	QTreeView*			_table{};
	QStandardItemModel*	_model{};
	DataType			_data;
	DataType			_dataSave;
	ColorScheme			_currentScheme;

	void choosedLine(const QItemSelection& selected, const QItemSelection& deselected);
	bool isUniqName(const QString & name, int index = -1) const;
	void appendTable();

	void closeEvent(QCloseEvent *event) override;

	int closeMsgBox();
};
