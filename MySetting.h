#ifndef MYSETTING_H
#define MYSETTING_H

#include <QSettings>
#include <QDir>

// a framework supporting multi-user ini file
template <class T>
class MySetting : public QSettings
{
	typedef std::map<QString, T*> Manager;

public:
	QString getFileName() const { return fileName; }
	void saveTo(const QString& file);

	static QString findFile(const QString& section, const QVariant& v);
	static T*      getInstance(const QString& fileName = "Global.ini");
	static void    destroySettingManager();

protected:
	MySetting(const QString& name);
	~MySetting() {}

	virtual void loadDefaults() = 0;

protected:
	static Manager settingManager;
	QString fileName;
};

template <class T>
MySetting<T>::MySetting(const QString& name) 
: QSettings(name, IniFormat), fileName(name) {}

template <class T>
typename MySetting<T>::Manager MySetting<T>::settingManager;

template <class T>
T* MySetting<T>::getInstance(const QString& name)
{
	QString fileName(name);
#ifdef Q_WS_WIN
	fileName.prepend("WIN_");
#endif

#ifdef Q_WS_MAC
	fileName.prepend("MAC_");
#endif

	typename Manager::iterator it = settingManager.find(fileName);
	if(it != settingManager.end())
		return it->second;

	T* setting = new T(fileName);
	settingManager.insert(std::make_pair(fileName, setting));
	return setting;
}

// search for user with specific section-value
template <class T>
QString MySetting<T>::findFile(const QString& section, const QVariant& v)
{
	// search all ini files
	const QStringList files = QDir().entryList(QStringList() << "*.ini", QDir::Files);
	foreach(QString file, files)
	{
		QSettings setting(file, QSettings::IniFormat);
		if(setting.value(section) == v)
			return file;
	}
	return QString();
}

template <class T>
void MySetting<T>::destroySettingManager()
{
	for(typename Manager::iterator it = settingManager.begin();
		it != settingManager.end(); ++it)
		delete it->second;
	settingManager.clear();
}

template <class T>
void MySetting<T>::saveTo(const QString& file)
{
	MySetting<T> other(file);
	QStringList keys = allKeys();
	foreach(QString key, keys)
		other.setValue(key, value(key));
}

#endif // MYSETTING_H
