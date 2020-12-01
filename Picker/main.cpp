#include "Picker.h"
#include "ColorSchemesWidget.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	ColorSchemesWidget w;
	//Picker w;
	w.show();

	return app.exec();
}
