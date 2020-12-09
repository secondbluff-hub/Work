#include "ColorSchemeModel.h"

#include <QtWidgets/QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	ColorSchemeModel m("filename.data");

	qDebug() << m.chooseScheme().size();

	return 0;
}
