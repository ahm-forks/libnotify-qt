#include "libnotify-qt.h"

#include <QImage>
#include <QDBusInterface>
#include <QDBusConnection>

#include "Interface.h"

enum class ServerInfo: int {
	NAME = 0,
	VENDOR,
	VERSION
};

using namespace Notification;
Manager::Manager(const QString & appName, QObject * parent) :
	QObject(parent),
	appName(appName),
	ids()
{
	start();
}

Manager::~Manager()
{
	stop();
}

bool Manager::ping()
{
	if(INotifications.isNull()) return false;
	return INotifications->connection().isConnected();
}

const QString & Manager::getAppName()
{
	return appName;
}

EventPtr Manager::createNotification(const QString & summary, const QString & body,
                                     const QString & iconName)
{
	EventPtr ntf = EventPtr(new Event(*this, summary, body, iconName));
	return ntf;
}

void Manager::addNotification(EventPtr notif, quint32 id)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	if(ids.contains(id)) {
		auto ntf = ids.take(id);
		ntf->emitClosed(0);
	}
	ids.insert(id, notif);
#else
	ids.insertOrAssign(id, notif);
#endif
}

void Manager::onActionInvoked(quint32 id, const QString & actionKey)
{
	if(! ids.contains(id) || ids.value(id).isNull()) return;
	ids.value(id)->emitAction(actionKey);
}

void Manager::onNotificationClosed(quint32 id, quint32 reason)
{
	if(INotifications.isNull()) return;
	if(! ids.contains(id) || ids.value(id).isNull()) return;
	EventPtr notif  = ids.take(id);
	notif->emitClosed(reason);
}

bool Manager::start()
{
	if(!QDBusConnection::sessionBus().isConnected())
		return false;

	INotifications = QSharedPointer<org::freedesktop::Notifications>(
	                     new org::freedesktop::Notifications(
	                         "org.freedesktop.Notifications",
	                         "/org/freedesktop/Notifications",
	                         QDBusConnection::sessionBus()
	                     ));
	if(INotifications->isValid()) {
		connect(INotifications.get(), &org::freedesktop::Notifications::ActionInvoked,
		        this, &Manager::onActionInvoked);

		connect(INotifications.get(), &org::freedesktop::Notifications::NotificationClosed,
		        this, &Manager::onNotificationClosed);
		return true;
	} else {
		INotifications.clear();
		return false;
	}
}


void Manager::stop()
{
	if(INotifications.isNull()) return;

	for(auto i = ids.begin(); i != ids.end(); ++i)
		onNotificationClosed(i.key(), 0);

	disconnect(INotifications.get());
	INotifications.clear();
}


QStringList Manager::getServerCaps()
{
	if(INotifications.isNull()) return QStringList();

	QDBusPendingReply<QStringList> reply = INotifications->getCapabilities();
	reply.waitForFinished();

	if(reply.isValid()) {
		return reply.argumentAt(0).toStringList();
	}

	return QStringList();
}

bool Manager::getServerInfo(QString & name, QString & vendor, QString & version)
{
	if(INotifications.isNull()) return false;

	QDBusPendingReply<QString, QString, QString> reply = INotifications->getServerInformation();
	reply.waitForFinished();

	if(reply.isValid()) {
		name = reply.argumentAt((int)ServerInfo::NAME).toString();
		vendor = reply.argumentAt((int)ServerInfo::VENDOR).toString();
		version = reply.argumentAt((int)ServerInfo::VENDOR).toString();
		return true;
	}

	return false;
}

bool Manager::show(
    const EventPtr& notif,
    quint32 & id)
{
	if(INotifications.isNull()) return false;

	QDBusPendingReply<quint32> reply = INotifications->notify(
	                                       appName, id,
	                                       notif->iconName(),
	                                       notif->summery(),
	                                       notif->body(),
	                                       notif->actions(),
	                                       notif->hints(),
	                                       notif->timeout()
	                                   );

	if(id == 0) {
		reply.waitForFinished();
		if(!reply.isValid())
			return false;

		id = reply.argumentAt(0).toInt();
		addNotification(notif, id);
	}

	return true;
}

bool Manager::close(quint32 & id)
{
	if(id == 0) return true;
	if(INotifications.isNull()) return false;
	if(! ids.contains(id) || ids.value(id).isNull()) return false;

	QDBusPendingReply<> reply = INotifications->closeNotification(id);
	reply.waitForFinished();
	return reply.isValid();
}

QVariant Manager::serializeImage(const QImage & img)
{
	QDBusArgument icon;
	icon.beginStructure();
	icon << img.width()
	     << img.height()
	     << (qint32) img.bytesPerLine() // rowstride
	     << img.hasAlphaChannel()
	     << 8 // bits_per_sample, always 8
	     << (img.hasAlphaChannel()?4:3)
	     << QByteArray::fromRawData((const char*) img.constBits(), img.sizeInBytes());
	icon.endStructure();
	return QVariant::fromValue(icon);
}
