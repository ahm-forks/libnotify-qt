/* libnotify-qt - library for sending notifications implemented in Qt
 * Copyright (C) 2010-2011 Vojtech Drbohlav <vojta.d@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Notification.h"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QImage>
#include <QPixmap>
#include <QHash>

#include "OrgFreedesktopNotificationsInterface.h"

NotificationManager::NotificationManager(const QString & appName, QObject * parent) :
	QObject(parent),
	appName(appName),
	ids()
{
	start();
}

bool NotificationManager::start()
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
		        this, &NotificationManager::onActionInvoked);

		connect(INotifications.get(), &org::freedesktop::Notifications::NotificationClosed,
		        this, &NotificationManager::onNotificationClosed);
		return true;
	} else {
		INotifications.clear();
		return false;
	}
}


void NotificationManager::stop()
{
	if(INotifications.isNull()) return;

	for(auto i = ids.begin(); i != ids.end(); ++i)
		onNotificationClosed(i.key(), 0);

	disconnect(INotifications.get());
	INotifications.clear();
}


NotificationManager::~NotificationManager()
{
	stop();
}

bool NotificationManager::ping()
{
	if(INotifications.isNull()) return false;
	return INotifications->connection().isConnected();
}

const QString & NotificationManager::getAppName()
{
	return appName;
}

QSharedPointer<Notification> NotificationManager::createNotification(const QString & summary, const QString & body,
        const QString & iconName)
{
	QSharedPointer<Notification> ntf = QSharedPointer<Notification>(
	                                       new Notification(*this, summary, body, iconName));
	return ntf;
}

void NotificationManager::addNotification(QSharedPointer<Notification> notif, quint32 id)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	if(ids.contains(id)) {
		auto ntf = ids.take(id);
		ntf->onNotificationClosed(0);
	}
	ids.insert(id, notif);
#else
	ids.insertOrAssign(id, notif);
#endif
}

void NotificationManager::onActionInvoked(quint32 id, const QString & actionKey)
{
	if(! ids.contains(id) || ids.value(id).isNull()) return;
	ids.value(id)->onActionInvoked(actionKey);
}

void NotificationManager::onNotificationClosed(quint32 id, quint32 reason)
{
	if(INotifications.isNull()) return;
	if(! ids.contains(id) || ids.value(id).isNull()) return;
	QSharedPointer<Notification> notif  = ids.take(id);
	notif->onNotificationClosed(reason);
}

QStringList NotificationManager::getServerCaps()
{
	if(INotifications.isNull()) return QStringList();

	QDBusPendingReply<QStringList> reply = INotifications->getCapabilities();
	reply.waitForFinished();

	if(reply.isValid()) {
		return reply.argumentAt(0).toStringList();
	}

	return QStringList();
}

bool NotificationManager::getServerInfo(QString & name, QString & vendor, QString & version)
{
	if(INotifications.isNull()) return false;

	QDBusPendingReply<QString, QString, QString> reply = INotifications->getServerInformation();
	reply.waitForFinished();

	if(reply.isValid()) {
		name = reply.argumentAt(SERVER_INFO_NAME).toString();
		vendor = reply.argumentAt(SERVER_INFO_VENDOR).toString();
		version = reply.argumentAt(SERVER_INFO_VERSION).toString();
		return true;
	}

	return false;
}

bool NotificationManager::show(
    const QSharedPointer<Notification>& notif,
    quint32 & id, const QString & appIcon,
    const QString & summary, const QString & body,
    const QStringList & actions, const QVariantMap & hints,
    qint32 timeout)
{
	if(INotifications.isNull()) return false;

	QDBusPendingReply<quint32> reply = INotifications->notify(appName, id, appIcon, summary, body,
	                                   actions, hints, timeout);
	if(id == 0) {
		reply.waitForFinished();
		if(!reply.isValid())
			return false;

		id = reply.argumentAt(0).toInt();
		addNotification(notif, id);
	}

	return true;
}

bool NotificationManager::close(quint32 & id)
{
	if(id == 0) return true;
	if(INotifications.isNull()) return false;
	if(! ids.contains(id) || ids.value(id).isNull()) return false;

	QDBusPendingReply<> reply = INotifications->closeNotification(id);
	reply.waitForFinished();
	return reply.isValid();
}

Notification::Notification(
    NotificationManager& parent,
    const QString & summary,
    const QString & body,
    const QString & iconName) :
	QObject(&parent),
	mgr(parent),
	m_id(0),
	m_summary(summary),
	m_body(body),
	m_iconName(iconName),
	m_timeout(5000),
	m_autoDelete(true)
{
	setUrgency(NOTIFICATION_URGENCY_NORMAL);
}

bool Notification::show()
{
	return mgr.show(this->sharedFromThis(), m_id, m_iconName, m_summary, m_body, m_actions, m_hints, m_timeout);
}

Notification* Notification::setSummary(const QString & summary)
{
	m_summary = summary;
	return this;
}

Notification* Notification::setBody(const QString & body)
{
	m_body = body;
	return this;
}

Notification* Notification::setIconName(const QString & iconName)
{
	m_iconName = iconName;
	return this;
}

Notification* Notification::setTimeout(qint32 timeout)
{
	m_timeout = timeout;
	return this;
}

Notification* Notification::setUrgency(NotificationUrgency urgency)
{
	return setHintByte("urgency", urgency);
}

Notification* Notification::setCategory(const QString & category)
{
	return setHintString("category", category);
}

Notification* Notification::setIconFromPixmap(const QPixmap & pixmap)
{
	return setIconFromImage(pixmap.toImage());
}

Notification* Notification::setIconFromImage(const QImage & img)
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
	return setHint("image-data", QVariant::fromValue(icon));
}

Notification* Notification::setLocation(qint32 x, qint32 y)
{
	return (setHintInt32("x", x)->setHintInt32("y", y));
}

Notification* Notification::setHint(const QString & key, const QVariant & value)
{
	m_hints.insert(key, value);
	return this;
}

Notification* Notification::setHintInt32(const QString & key, qint32 value)
{
	m_hints.insert(key, value);
	return this;
}

Notification* Notification::setHintDouble(const QString & key, double value)
{
	m_hints.insert(key, value);
	return this;
}

Notification* Notification::setHintString(const QString & key, const QString & value)
{
	m_hints.insert(key, value);
	return this;
}

Notification* Notification::setHintByte(const QString & key, char value)
{
	return setHintByteArray(key, QByteArray(1, value));
}

Notification* Notification::setHintByteArray(const QString & key, const QByteArray & value)
{
	m_hints.insert(key, value);
	return this;
}

Notification* Notification::clearHints()
{
	m_hints.clear();
	return this;
}

Notification* Notification::addAction(const QString & actionKey, const QString & label)
{
	m_actions << actionKey << label;
	return this;
}

Notification* Notification::clearActions()
{
	m_actions.clear();
	return this;
}

bool Notification::close()
{
	return mgr.close(m_id);
}

bool Notification::autoDelete() const
{
	return m_autoDelete;
}

Notification* Notification::setAutoDelete(bool autoDelete)
{
	m_autoDelete = autoDelete;
	return this;
}

void Notification::onNotificationClosed(quint32 reason)
{
	emit closed(reason);
	if(m_autoDelete)
		deleteLater();
}

void Notification::onActionInvoked(const QString & actionKey)
{
	emit actionInvoked(actionKey);
}
