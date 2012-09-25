#include "Setting.h"
#include "Defines.h"
#include <QColor>
#include <QResource>

namespace TeamRadar {

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
	// server
	setServerAddress("127.0.0.1");
	setServerPort(12345);

	// client
	setUserName(guessUserName());
	setRootPath(QDir::currentPath());
	setValue("UseEffects", false);   // fancy visual effects: blur, shadow
	setValue("AfterImageDuration", 20);
	setValue("DefaultDeveloperImage", ":/Images/Head_Male.png");
	setValue("PhotoPath",       "./Photos");
	setValue("ChatHistoryPath", "./ChatHistory");
	setFontSize(10);
	setValue("PartitionFuzziness", 100);

	// parameters for the engine
#ifdef OS_DESKTOP
	setEngineSubtlety(1);
	setShowLightTrail(false);
	setShowAfterImage(false);
#else
	setEngineSubtlety(10);
	setShowLightTrail(true);
	setShowAfterImage(true);
#endif

	// filtered trash files/dirs
	setValue("FilteredDirs",  "Debug;Release;Temp;Tmp;Lib;libs");
	setValue("FilteredFiles", "obj;tmp;dll;pdb;o;ilk;idb");

	// colors
	setColor("EdgeColor",             QColor(Qt::black));
	setColor("BackgroundColor",       QColor(Qt::white));   // scene color
    setColor("RootColor",             QColor(Qt::darkYellow));
	setColor("DefaultDirColor",       QColor(255, 205, 5));
	setColor("DefaultFileColor",      QColor(Qt::darkGray));
	setColor("DefaultDeveloperColor", QColor(Qt::gray));

	// extension colors
    setExtensionColor("h",    QColor(Qt::green));
    setExtensionColor("hpp",  QColor(Qt::green));

    setExtensionColor("c",    QColor(Qt::darkGreen));
    setExtensionColor("cpp",  QColor(Qt::darkGreen));

	setExtensionColor("txt",  QColor(155, 187, 89));
	setExtensionColor("htm",  QColor(155, 187, 89));
	setExtensionColor("html", QColor(155, 187, 89));

    setExtensionColor("pro", QColor(40, 166, 198));

	setExtensionColor("ui",  QColor(128, 100, 162));
	setExtensionColor("qrc", QColor(128, 100, 162));

	setExtensionColor("png",  QColor(255, 102, 204));
	setExtensionColor("ico",  QColor(255, 102, 204));
	setExtensionColor("icns", QColor(255, 102, 204));
	setExtensionColor("jpg",  QColor(255, 102, 204));

	setTags(getDefaultTags());
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

qreal Setting::getThreshold() const {
	return 1.0 / getEngineSubtlety();   // 0.1~1
}

void Setting::setFontSize(int size) {
	setValue("FontSize", size);
}

int Setting::getFontSize() const {
	return value("FontSize").toInt();
}

bool Setting::showLightTrail() const {
	return value("LightTrail").toBool();
}

bool Setting::showAfterImage() const {
	return value("AfterImage").toBool();
}

void Setting::setShowLightTrail(bool show) {
	setValue("LightTrail", show);
}

void Setting::setShowAfterImage(bool show) {
	setValue("AfterImage", show);
}

int Setting::getEngineRate() const {
	return getEngineSubtlety() * 10;   // 10~100 msecs
}

int Setting::getEngineSubtlety() const {
	return value("EngineSubtlety").toInt();   // 1~10
}

void Setting::setEngineSubtlety(int subtlety) {
	setValue("EngineSubtlety", subtlety);
}

QString Setting::getCompileDate() const
{
	// this resource file will be generated after running CompileDate.bat
	QResource resource(":/CompileDate.txt");
	QString result = (char*)resource.data();
	return result.isEmpty() ? "Unknown" : result;
}

// search the environmental variables for user name
QString Setting::guessUserName() const
{
	QStringList envVariables;
	envVariables << "USERNAME.*" << "USER.*" << "USERDOMAIN.*"
				 << "HOSTNAME.*" << "DOMAINNAME.*";
	QString result;
	QStringList environment = QProcess::systemEnvironment();
	foreach(QString string, envVariables)
	{
		int index = environment.indexOf(QRegExp(string));
		if(index != -1)
		{
			QStringList stringList = environment.at(index).split('=');
			if(stringList.size() == 2)
			{
				result = stringList.at(1).toUtf8();
				break;
			}
		}
	}

	return result.isEmpty() ? "Unknown" : result;
}

void Setting::setTags(const TagKeywords& tags) {
	setValue("TodoTag/TagKeywords", QVariant::fromValue(tags));
}

TagKeywords Setting::getTags() const {
	return value("TodoTag/TagKeywords").value<TagKeywords>();
}

TagKeywords Setting::getDefaultTags() const
{
	TagKeywords result;
	result.insert("NOTE",     TagKeyword("NOTE",     QIcon(":/Images/Information"), QColor("#E2DFFF")));
	result.insert("INFO",     TagKeyword("INFO",     QIcon(":/Images/Information"), QColor("#E2DFFF")));
	result.insert("TODO",     TagKeyword("TODO",     QIcon(":/Images/Todo"),        QColor("#BFFFC8")));
	result.insert("WARNING",  TagKeyword("WARNING",  QIcon(":/Images/Warning"),     QColor("#FFFFAA")));
	result.insert("FIXME",    TagKeyword("FIXME",    QIcon(":/Images/Bug"),         QColor("#FFDFDF")));
	result.insert("BUG",      TagKeyword("BUG",      QIcon(":/Images/Bug"),         QColor("#FFDFDF")));
	result.insert("Critical", TagKeyword("Critical", QIcon(":/Images/Critical"),    QColor("#FFBFBF")));
	return result;
}

} // namespace TeamRadar
