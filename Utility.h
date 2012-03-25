#ifndef UTILITY_H
#define UTILITY_H
#include <QColor>
#include <QPointF>
#include <QString>

namespace TeamRadar {

QColor complementColor(const QColor& color);
QPointF generateHeavenPos(int distance = 1000, double angle = 0.0);
QString toAbsolutePath(const QString& relativePath);
QString toRelativePath(const QString& absolutePath);
bool isFilteredDir (const QString& dirName);
bool isFilteredFile(const QString& fileName);
QString getFirstSection(const QString& path);
QString toStandardPath(const QString& path);  // \ -> /

} // namespace TeamRadar

#endif // UTILITY_H
