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

#ifndef LIBNOTIFY_QT_H
#define LIBNOTIFY_QT_H

#include <QObject>
#include <QHash>
#include <QStringList>
#include <QVariantMap>
#include <QSharedPointer>

namespace org
{
	namespace freedesktop
	{
        class Notifications;
	}
}

namespace Notification {
    class Manager;
    class Event;
    enum class Urgency: quint8;
    using EventPtr = QSharedPointer<Event>;
}

enum class Notification::Urgency: quint8
{
    LOW,
    NORMAL,
    CRITICAL
};

class Q_DECL_EXPORT Notification::Manager: public QObject {
    Q_OBJECT

public:
    static QVariant serializeImage(const QImage & img);

public:
    const QString appName;

    Manager(const QString & appName, QObject * parent = 0);
    ~Manager();

    bool start();
    void stop();
    bool ping();
    const QString & getAppName();
    QStringList getServerCaps();
    bool getServerInfo(QString & name, QString & vendor, QString & version);
    EventPtr createNotification(const QString & summary, const QString & body = QString(),
                                                    const QString & iconName = QString());
    bool show(const EventPtr & notif, quint32 & id);
    bool close(quint32 & id);


private:
    QSharedPointer<org::freedesktop::Notifications> INotifications = nullptr;
    QHash<quint32, EventPtr> ids;
    void addNotification(EventPtr notif, quint32 id);

private slots:
    void onNotificationClosed(quint32 id, quint32 reason);
    void onActionInvoked(quint32 id, const QString & actionKey);
};

class Q_DECL_EXPORT Notification::Event : public QObject, public QEnableSharedFromThis<Event>
{
    Q_OBJECT

public:
        Event(Manager& parent,
                     const QString & summary,
                     const QString & body = QString(),
                     const QString & iconName = QString());

		bool show();
		bool close();
		bool autoDelete() const;

        Event* setAutoDelete(bool autoDelete);
        Event* setSummary(const QString & summary);
        Event* setBody(const QString & body);
        Event* setIconName(const QString & iconName);
        Event* setTimeout(qint32 timeout);
        Event* setUrgency(Urgency urgency);
        Event* setCategory(const QString & category);
        Event* setIconFromPixmap(const QPixmap & img);
        Event* setIconFromImage(const QImage & img);
        Event* setLocation(qint32 x, qint32 y);

        Event* setHint(const QString & key, const QVariant & value);
        Event* setHintInt32(const QString & key, qint32 value);
        Event* setHintDouble(const QString & key, double value);
        Event* setHintString(const QString & key, const QString & value);
        Event* setHintByte(const QString & key, char value);
        Event* setHintByteArray(const QString & key, const QByteArray & value);
        Event* clearHints();

        Event* addAction(const QString & actionKey, const QString & label);
        Event* clearActions();
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
        Manager& mgr;
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
