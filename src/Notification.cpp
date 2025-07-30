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

#include <QImage>
#include <QPixmap>
#include <QHash>

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
	setUrgency(NotificationUrgency::NORMAL);
}

bool Notification::show()
{
	return mgr.show(this->sharedFromThis(), m_id);
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
	return setHintByte("urgency", (quint8) urgency);
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
	return setHint("image-data", NotificationManager::serializeImage(img));
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

quint32 Notification::timeout()
{
	return m_timeout;
}

const QString & Notification::summery()
{
	return m_summary;
}

const QString & Notification::body()
{
	return m_body;
}
const QString & Notification::iconName()
{
	return m_iconName;
}

const QStringList & Notification::actions()
{
	return m_actions;
}

const QVariantMap & Notification::hints()
{
	return m_hints;
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

void Notification::emitClosed(quint32 reason)
{
	emit closed(reason);
	if(m_autoDelete)
		deleteLater();
}

void Notification::emitAction(const QString & actionKey)
{
	emit actionInvoked(actionKey);
}
