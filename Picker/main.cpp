#include "ColorSchemeModel.h"

#include <QtWidgets/QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	ColorSchemeModel m("filename.data");

	//m.addScheme({ "tester", { {1, Qt::yellow} } });

	qDebug() << m.chooseScheme(new QWidget()).size();

	return 0;
}
