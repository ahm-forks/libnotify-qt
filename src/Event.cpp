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


#include "libnotify-qt_p.h"
#include "logging.h"

#include <QImage>
#include <QPixmap>
#include <QHash>

using namespace Notification;

Q_LOGGING_CATEGORY(Levt, "qt.xdg.notification.event");

Event::Event(
    Manager& parent,
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
	qCDebug(Levt) << "Notification created (Manager: " << &mgr << ", instance: " << this << ")";
	setUrgency(Urgency::NORMAL);
}

bool Event::show()
{
	return mgr.show(this->sharedFromThis(), m_id);
}

EventPtr Event::setSummary(const QString & summary)
{
	m_summary = summary;
	return sharedFromThis();
}

EventPtr Event::setBody(const QString & body)
{
	m_body = body;
	return sharedFromThis();
}

EventPtr Event::setIconName(const QString & iconName)
{
	m_iconName = iconName;
	return sharedFromThis();
}

EventPtr Event::setTimeout(qint32 timeout)
{
	m_timeout = timeout;
	return sharedFromThis();
}

EventPtr Event::setUrgency(Urgency urgency)
{
	return setHintByte("urgency", (quint8) urgency);
}

EventPtr Event::setCategory(const QString & category)
{
	return setHintString("category", category);
}

EventPtr Event::setIconFromPixmap(const QPixmap & pixmap)
{
	return setIconFromImage(pixmap.toImage());
}

EventPtr Event::setIconFromImage(const QImage & img)
{
	return setHint("image-data", Manager::serializeImage(img));
}

EventPtr Event::setLocation(qint32 x, qint32 y)
{
	return (setHintInt32("x", x)->setHintInt32("y", y));
}

EventPtr Event::setHint(const QString & key, const QVariant & value)
{
	m_hints.insert(key, value);
	return sharedFromThis();
}

EventPtr Event::setHintInt32(const QString & key, qint32 value)
{
	m_hints.insert(key, value);
	return sharedFromThis();
}

EventPtr Event::setHintDouble(const QString & key, double value)
{
	m_hints.insert(key, value);
	return sharedFromThis();
}

EventPtr Event::setHintString(const QString & key, const QString & value)
{
	m_hints.insert(key, value);
	return sharedFromThis();
}

EventPtr Event::setHintByte(const QString & key, char value)
{
	return setHintByteArray(key, QByteArray(1, value));
}

EventPtr Event::setHintByteArray(const QString & key, const QByteArray & value)
{
	m_hints.insert(key, value);
	return sharedFromThis();
}

EventPtr Event::clearHints()
{
	m_hints.clear();
	return sharedFromThis();
}

EventPtr Event::addAction(const QString & actionKey, const QString & label)
{
	m_actions << actionKey << label;
	return sharedFromThis();
}

EventPtr Event::clearActions()
{
	m_actions.clear();
	return sharedFromThis();
}

quint32 Event::timeout()
{
	return m_timeout;
}

const QString & Event::summary()
{
	return m_summary;
}

const QString & Event::body()
{
	return m_body;
}
const QString & Event::iconName()
{
	return m_iconName;
}

const QStringList & Event::actions()
{
	return m_actions;
}

const QVariantMap & Event::hints()
{
	return m_hints;
}


bool Event::close()
{
	return mgr.close(m_id);
}

bool Event::autoDelete() const
{
	return m_autoDelete;
}

EventPtr Event::setAutoDelete(bool autoDelete)
{
	m_autoDelete = autoDelete;
	return sharedFromThis();
}

void Event::emitClosed(ClosingReason reason)
{
	emit closed(reason);
	if(m_autoDelete)
		deleteLater();

	qCDebug(Levt) << "Notification closed (Manager: " << &mgr << ", instance: " << this << ")";
}

void Event::emitAction(const QString & actionKey)
{
	emit actionInvoked(actionKey);
}

void Event::emitToken(const QString & token)
{
	emit activationToken(token);
}
