#include "TaoItem.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QSurfaceFormat>
int main(int argc, char* argv[])
{
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	QGuiApplication app(argc, argv);
	app.setOrganizationName("Tao");
	app.setOrganizationDomain("Tao");

    auto format = QSurfaceFormat::defaultFormat();
    format.setProfile(QSurfaceFormat::CoreProfile);
#ifdef DEBUG_GL    
    format.setOption(QSurfaceFormat::DebugContext);
#endif    
    QSurfaceFormat::setDefaultFormat(format);

	qmlRegisterType<TaoItem>("TaoItem", 1, 0, "TaoItem");

	QQmlApplicationEngine engine;
	engine.load(QUrl(QStringLiteral("qrc:/Qml/main.qml")));
	if (engine.rootObjects().isEmpty())
		return -1;

	return app.exec();
}
