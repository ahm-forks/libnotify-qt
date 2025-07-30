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

#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QObject>
#include <QHash>
#include <QStringList>
#include <QVariantMap>
#include <QSharedPointer>

enum ServerInfo
{
	SERVER_INFO_NAME = 0,
	SERVER_INFO_VENDOR,
	SERVER_INFO_VERSION
};

enum NotificationUrgency
{
	NOTIFICATION_URGENCY_LOW,
	NOTIFICATION_URGENCY_NORMAL,
	NOTIFICATION_URGENCY_CRITICAL
};

namespace org
{
	namespace freedesktop
	{
        class Notifications;
	}
}

class Q_DECL_EXPORT Notification;

class Q_DECL_EXPORT NotificationManager: public QObject {
    Q_OBJECT

public:
    static QVariant serializeImage(const QImage & img);

public:
    const QString appName;

    NotificationManager(const QString & appName, QObject * parent = 0);
    ~NotificationManager();

    bool start();
    void stop();
    bool ping();
    const QString & getAppName();
    QStringList getServerCaps();
    bool getServerInfo(QString & name, QString & vendor, QString & version);
    QSharedPointer<Notification> createNotification(const QString & summary, const QString & body = QString(),
                                                    const QString & iconName = QString());
    bool show(const QSharedPointer<Notification> & notif, quint32 & id);
    bool close(quint32 & id);


private:
    QSharedPointer<org::freedesktop::Notifications> INotifications = nullptr;
    QHash<quint32, QSharedPointer<Notification>> ids;
    void addNotification(QSharedPointer<Notification> notif, quint32 id);

private slots:
    void onNotificationClosed(quint32 id, quint32 reason);
    void onActionInvoked(quint32 id, const QString & actionKey);
};

class Q_DECL_EXPORT Notification : public QObject, public QEnableSharedFromThis<Notification>
{
    Q_OBJECT

public:
        Notification(NotificationManager& parent,
                     const QString & summary,
                     const QString & body = QString(),
                     const QString & iconName = QString());

		bool show();
		bool close();
		bool autoDelete() const;

		Notification* setAutoDelete(bool autoDelete);
		Notification* setSummary(const QString & summary);
		Notification* setBody(const QString & body);
		Notification* setIconName(const QString & iconName);
		Notification* setTimeout(qint32 timeout);
		Notification* setUrgency(NotificationUrgency urgency);
		Notification* setCategory(const QString & category);
        Notification* setIconFromPixmap(const QPixmap & img);
        Notification* setIconFromImage(const QImage & img);
		Notification* setLocation(qint32 x, qint32 y);

		Notification* setHint(const QString & key, const QVariant & value);
		Notification* setHintInt32(const QString & key, qint32 value);
		Notification* setHintDouble(const QString & key, double value);
		Notification* setHintString(const QString & key, const QString & value);
		Notification* setHintByte(const QString & key, char value);
		Notification* setHintByteArray(const QString & key, const QByteArray & value);
		Notification* clearHints();

		Notification* addAction(const QString & actionKey, const QString & label);
		Notification* clearActions();
        void emitClosed(quint32 reason);
        void emitAction(const QString & actionKey);

    public:
        quint32   timeout();
        const QString & summery();
        const QString & body();
        const QString & iconName();
        const QStringList & actions();
        const QVariantMap & hints();
    private:
        NotificationManager& mgr;
		quint32 m_id;
		QString m_summary;
		QString m_body;
		QString m_iconName;
		qint32 m_timeout;
		QStringList m_actions;
        QVariantMap m_hints;

		bool m_autoDelete;

	signals:
		void closed(quint32 reason);
		void actionInvoked(const QString & actionKey);
};

#endif // NOTIFICATION_H
