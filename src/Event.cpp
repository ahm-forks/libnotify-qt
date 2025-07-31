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

#include <QImage>
#include <QPixmap>
#include <QHash>

using namespace Notification;

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
	setUrgency(Urgency::NORMAL);
}

bool Event::show()
{
	return mgr.show(this->sharedFromThis(), m_id);
}

Event* Event::setSummary(const QString & summary)
{
	m_summary = summary;
	return this;
}

Event* Event::setBody(const QString & body)
{
	m_body = body;
	return this;
}

Event* Event::setIconName(const QString & iconName)
{
	m_iconName = iconName;
	return this;
}

Event* Event::setTimeout(qint32 timeout)
{
	m_timeout = timeout;
	return this;
}

Event* Event::setUrgency(Urgency urgency)
{
	return setHintByte("urgency", (quint8) urgency);
}

Event* Event::setCategory(const QString & category)
{
	return setHintString("category", category);
}

Event* Event::setIconFromPixmap(const QPixmap & pixmap)
{
	return setIconFromImage(pixmap.toImage());
}

Event* Event::setIconFromImage(const QImage & img)
{
	return setHint("image-data", Manager::serializeImage(img));
}

Event* Event::setLocation(qint32 x, qint32 y)
{
	return (setHintInt32("x", x)->setHintInt32("y", y));
}

Event* Event::setHint(const QString & key, const QVariant & value)
{
	m_hints.insert(key, value);
	return this;
}

Event* Event::setHintInt32(const QString & key, qint32 value)
{
	m_hints.insert(key, value);
	return this;
}

Event* Event::setHintDouble(const QString & key, double value)
{
	m_hints.insert(key, value);
	return this;
}

Event* Event::setHintString(const QString & key, const QString & value)
{
	m_hints.insert(key, value);
	return this;
}

Event* Event::setHintByte(const QString & key, char value)
{
	return setHintByteArray(key, QByteArray(1, value));
}

Event* Event::setHintByteArray(const QString & key, const QByteArray & value)
{
	m_hints.insert(key, value);
	return this;
}

Event* Event::clearHints()
{
	m_hints.clear();
	return this;
}

Event* Event::addAction(const QString & actionKey, const QString & label)
{
	m_actions << actionKey << label;
	return this;
}

Event* Event::clearActions()
{
	m_actions.clear();
	return this;
}

quint32 Event::timeout()
{
	return m_timeout;
}

const QString & Event::summery()
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

Event* Event::setAutoDelete(bool autoDelete)
{
	m_autoDelete = autoDelete;
	return this;
}

void Event::emitClosed(quint32 reason)
{
	emit closed(reason);
	if(m_autoDelete)
		deleteLater();
}

void Event::emitAction(const QString & actionKey)
{
	emit actionInvoked(actionKey);
}
