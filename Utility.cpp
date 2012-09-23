#include "Utility.h"
#include "Setting.h"
#include "Connection.h"
#include <math.h>
#include <QFile>
#include <QFileInfo>
#include <coreplugin/icore.h>
#include <coreplugin/editormanager/editormanager.h>

namespace TeamRadar {

QColor complementColor(const QColor& color) {
	return QColor(255 - color.red(), 255 - color.green(), 255 - color.blue());
}

QPointF generateHeavenPos(int distance, double angle)
{
	double theta = angle == 0.0 ? (qrand() % 20) * 0.314 : angle;
	return QPointF(sin(theta) * distance, cos(theta) * distance);
}

// suppose the root path = c:/abc, relativePath = /d/e/f.h
// return c:/abc/d/e/f.h
QString toAbsolutePath(const QString& relativePath)
{
	QString rootPath = Setting::getInstance()->getRootPath();
	if(relativePath == rootPath)
		return relativePath;

	if(QFileInfo(relativePath).isAbsolute())  // already absolute
		return relativePath;
	return rootPath + '/' + relativePath;
}

// suppose the root path = c:/abc, absolutePath = c:/abc/d/e/f.h
// return /d/e/f.h
QString toRelativePath(const QString& absolutePath)
{
	if(!QFileInfo(absolutePath).isAbsolute())   // already relative
		return absolutePath;

	QString result = absolutePath;
	QString rootPath = Setting::getInstance()->getRootPath();
	result.remove(rootPath);
	return result.right(result.length() - 1);   // remove the first '/'
}

bool isFilteredDir(const QString& dirName)
{
	static QStringList filteredDirs = Setting::getInstance()->value("FilteredDirs").toString().split(';');
	return filteredDirs.contains(dirName, Qt::CaseInsensitive);
}

bool isFilteredFile(const QString& fileName)
{
	static QStringList filteredFiles = Setting::getInstance()->value("FilteredFiles").toString().split(';');
	return filteredFiles.contains(QFileInfo(fileName).suffix(), Qt::CaseInsensitive);
}

// if path = a/b/c.h, return a
QString getFirstSection(const QString& path)
{
	int index = path.indexOf('/');
	return index == -1 ? path : path.left(index);
}

QString toStandardPath(const QString& path)
{
	QString result = path;
	result.replace('\\', '/');
	return result;
}

void updateWindowTitleAddition()
{
    Core::EditorManager* editorManager = Core::ICore::instance()->editorManager();
    QString titleAddition = editorManager->windowTitleAddition();
    QRegExp rx(" - *", Qt::CaseInsensitive, QRegExp::Wildcard);
    titleAddition.remove(rx);
    titleAddition.append(" - " + Setting::getInstance()->getUserName());
    editorManager->setWindowTitleAddition(titleAddition);
}

} // namespace TeamRadar
