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

#ifndef INTERFACE_H
#define INTERFACE_H

#include <QDBusAbstractInterface>
#include <QDBusPendingReply>

namespace org {
    namespace freedesktop {
        class Notifications;
    }
}

class org::freedesktop::Notifications : public QDBusAbstractInterface
{
	Q_OBJECT

	public:
        Notifications(const QString & service, const QString & path,
											 const QDBusConnection & connection, QObject * parent = 0);
        ~Notifications();

		static inline const char * staticInterfaceName()
		{
			return "org.freedesktop.Notifications";
		}

	public slots:
		QDBusPendingReply<QStringList> getCapabilities();
		QDBusPendingReply<quint32> notify(const QString & appName, quint32 replacesId, const QString & appIcon,
										  const QString & summary, const QString & body,
										  const QStringList & actions, const QVariantMap & hints,
										  qint32 timeout);
		QDBusPendingReply<> closeNotification(quint32 id);
		QDBusPendingReply<QString, QString, QString> getServerInformation();

	signals:
		void ActionInvoked(quint32 id, const QString &action_key);
	    void NotificationClosed(quint32 id, quint32 reason);
};

#endif // INTERFACE_P_H
