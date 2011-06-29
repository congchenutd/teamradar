#include "Utility.h"
#include "Setting.h"
#include <math.h>
#include <QFile>
#include <QFileInfo>

QColor complementColor(const QColor& color) {
	return QColor(255 - color.red(), 255 - color.green(), 255 - color.blue());
}

QPointF generateHeavenPos(int distance, double angle)
{
	double theta = angle == 0.0 ? (qrand() % 20) * 0.314 : angle;
	return QPointF(sin(theta) * distance, cos(theta) * distance);
}

QString toAbsolutePath(const QString& root, const QString& path)
{
	if(QFile::exists(path))  // path is absolute
		return QFileInfo(path).absoluteFilePath();
	return QFileInfo(root + "/" + path).absoluteFilePath();
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
