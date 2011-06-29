#ifndef UTILITY_H
#define UTILITY_H
#include <QColor>
#include <QPointF>
#include <QString>

QColor complementColor(const QColor& color);
QPointF generateHeavenPos(int distance = 1000, double angle = 0.0);
QString toAbsolutePath(const QString& root, const QString& path);
bool isFilteredDir (const QString& dirName);
bool isFilteredFile(const QString& fileName);

#endif // UTILITY_H
