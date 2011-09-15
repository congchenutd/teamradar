#ifndef UTILITY_H
#define UTILITY_H
#include <QColor>
#include <QPointF>
#include <QString>

QColor complementColor(const QColor& color);
QPointF generateHeavenPos(int distance = 1000, double angle = 0.0);
QString toAbsolutePath(const QString& relativePath);
bool isFilteredDir (const QString& dirName);
bool isFilteredFile(const QString& fileName);
QString toRelativePath(const QString& absolutePath);
QString getFirstSection(const QString& path);

#endif // UTILITY_H
