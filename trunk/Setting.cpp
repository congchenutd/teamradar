#include "Setting.h"
#include <QColor>

Setting::Setting(const QString& fileName) : MySetting<Setting>(fileName)
{
	if(QFile(this->fileName).size() == 0)   // no setting
		loadDefaults();

	QDir::current().mkdir(value("PhotoPath").toString());
	QDir::current().mkdir(value("ChatHistoryPath").toString());
}

// shadow colors are the complement colors of the text or the scene
void Setting::loadDefaults()
{
	setServerAddress("127.0.0.1");
	setServerPort(12345);

	setRootPath(QDir::currentPath());
	setValue("UseEffects", false);   // fancy visual effects: blur, shadow
	setValue("AfterImageDuration", 20);
	setValue("IconSet", "IconSet1");
	setValue("DefaultDeveloperImage", ":/Images/Head_Male.png");
	setValue("PhotoPath",       "./Photos");
	setValue("ChatHistoryPath", "./ChatHistory");

	setValue("FilteredDirs",  "Debug;Release;Temp;Tmp;Lib;libs");
	setValue("FilteredFiles", "obj;tmp;dll;pdb;o;ilk;idb");

	setColor("EdgeColor",             QColor(Qt::black));
	setColor("BackgroundColor",       QColor(Qt::white));   // scene color
	setColor("DefaultDirColor",       QColor(255, 205, 5));
	setColor("DefaultFileColor",      QColor(Qt::darkGray));
	setColor("DefaultDeveloperColor", QColor(Qt::gray));

	setExtensionColor("h",    QColor(40, 166, 198));
	setExtensionColor("hpp",  QColor(40, 166, 198));

	setExtensionColor("c",    QColor(255, 60, 0));
	setExtensionColor("cpp",  QColor(255, 60, 0));

	setExtensionColor("txt",  QColor(155, 187, 89));
	setExtensionColor("htm",  QColor(155, 187, 89));
	setExtensionColor("html", QColor(155, 187, 89));

	setExtensionColor("ui",  QColor(128, 100, 162));
	setExtensionColor("pro", QColor(128, 100, 162));
	setExtensionColor("qrc", QColor(128, 100, 162));

	setExtensionColor("png",  QColor(255, 102, 204));
	setExtensionColor("ico",  QColor(255, 102, 204));
	setExtensionColor("icns", QColor(255, 102, 204));
	setExtensionColor("jpg",  QColor(255, 102, 204));
}

QColor Setting::getColor(const QString& section) const {
	return value("Colors/" + section).toString();
}

void Setting::setColor(const QString& section, const QColor& color) {
	setValue("Colors/" + section, color.name());
}

QColor Setting::getExtensionColor(const QString& fileName) const
{
	QString section = "ExtensionColor/" + QFileInfo(fileName).suffix().toLower();
	return contains(section) ? value(section).toString() : getColor("DefaultFileColor");
}

void Setting::setExtensionColor(const QString& extension, const QColor& color) {
	setValue("ExtensionColor/" + extension, color.name());
}

QString Setting::getServerAddress() const {
	return value("ServerAddress").toString();
}

quint16 Setting::getServerPort() const {
	return value("ServerPort").toInt();
}

void Setting::setServerAddress(const QString& address) {
	setValue("ServerAddress", address);
}

void Setting::setServerPort(quint16 port) {
	setValue("ServerPort", port);
}

QString Setting::getUserName() const {
	return value("UserName").toString();
}

void Setting::setUserName(const QString& name) {
	setValue("UserName", name);
}

QString Setting::getPhotoFilePath(const QString& userName) const {
	return value("PhotoPath").toString() + "/" + userName + ".png";
}

QString Setting::getChatHistoryPath() const {
	return value("ChatHistoryPath").toString();
}

QString Setting::getRootPath() const {
	return value("RootPath").toString();
}

void Setting::setRootPath(const QString& path) {
	setValue("RootPath", path);
}


const QString Setting::dateTimeFormat = "yyyy-MM-dd HH:mm:ss";
