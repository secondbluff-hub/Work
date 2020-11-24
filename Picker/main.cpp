#include "Picker.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Picker w;
	w.show();
	return a.exec();
}
