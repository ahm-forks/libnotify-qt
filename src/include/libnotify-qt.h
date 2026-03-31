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

#ifndef LIBNOTIFY_QT_EXPORT
#define LIBNOTIFY_QT_EXPORT Q_DECL_IMPORT
#endif

namespace org
{
	namespace freedesktop
	{
		class Notifications;
	}
}

namespace Notification {
	class LIBNOTIFY_QT_EXPORT
		Manager;
	class LIBNOTIFY_QT_EXPORT
		Event;
	enum class LIBNOTIFY_QT_EXPORT
		Urgency: quint8;
	enum class LIBNOTIFY_QT_EXPORT
		ClosingReason: quint32;
	using EventPtr = QSharedPointer<Event>;
}

enum class Notification::ClosingReason: quint32 {
	EXPIRED = 1,   // timeout expired
	DISMISSED = 2, // dismissed by the user
	BUSCALL = 3,   // closed using CloseNotification call
	UNDEFINED = 4  // Undefined/reserved reasons
};

enum class Notification::Urgency: quint8
{
	LOW,
	NORMAL,
	CRITICAL
};

class Notification::Manager: public QObject {
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
	void onActionInvoked(quint32 id, const QString & actionKey);
	void onActivationToken(quint32 id, const QString & token);
	void onNotificationClosed(quint32 id, quint32 reason);
};

class Notification::Event : public QObject, public QEnableSharedFromThis<Event>
{
	Q_OBJECT

private:
		friend class Manager;
		Event(Manager& parent,
					 const QString & summary,
					 const QString & body = QString(),
					 const QString & iconName = QString());

public:
		bool show();
		bool close();
		bool autoDelete() const;

		EventPtr setAutoDelete(bool autoDelete);
		EventPtr setSummary(const QString & summary);
		EventPtr setBody(const QString & body);
		EventPtr setIconName(const QString & iconName);
		EventPtr setTimeout(qint32 timeout);
		EventPtr setUrgency(Urgency urgency);
		EventPtr setCategory(const QString & category);
		EventPtr setIconFromPixmap(const QPixmap & img);
		EventPtr setIconFromImage(const QImage & img);
		EventPtr setLocation(qint32 x, qint32 y);

		EventPtr setHint(const QString & key, const QVariant & value);
		EventPtr setHintInt32(const QString & key, qint32 value);
		EventPtr setHintDouble(const QString & key, double value);
		EventPtr setHintString(const QString & key, const QString & value);
		EventPtr setHintByte(const QString & key, char value);
		EventPtr setHintByteArray(const QString & key, const QByteArray & value);
		EventPtr clearHints();

		EventPtr addAction(const QString & actionKey, const QString & label);
		EventPtr clearActions();
		void emitClosed(ClosingReason reason);
		void emitAction(const QString & actionKey);
		void emitToken(const QString & token);

	public:
		quint32   timeout();
		const QString & summary();
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
		void closed(ClosingReason reason);
		void actionInvoked(const QString & actionKey);
		void activationToken(const QString & token);
};

#endif // NOTIFICATION_H
