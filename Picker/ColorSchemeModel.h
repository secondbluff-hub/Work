#pragma once

#include <QWidget>

#include <vector>
#include <map>
#include <utility>

class ColorSchemeModel : public QWidget
{
	Q_OBJECT

public:
	using DataType = std::vector<std::pair<QString, std::map<int, QColor>>>;

	ColorSchemeModel(QWidget *parent);
	~ColorSchemeModel();

public slots:
	DataType chooseScheme();

signals:


private:
	std::vector<DataType> colorSchemes;

private slots:
	void addSequence(DataType data);
};
