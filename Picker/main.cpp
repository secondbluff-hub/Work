#include "Picker.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	Picker w;
	w.show();

	return app.exec();
}
