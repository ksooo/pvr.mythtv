/*
 *      Copyright (C) 2022 Jean-Luc Barriere
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
 *  MA 02110-1301 USA
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include "ringbuffer.h"

#include "os/threads/mutex.h"
#include "debug.h"

using namespace NSROOT;

namespace NSROOT
{
  struct RingBuffer::Lockable
  {
    OS::CMutex mutex;
  };
}

RingBufferPacket::RingBufferPacket(int _capacity)
: id(0)
, size(_capacity)
, data(new char [_capacity])
, capacity(_capacity)
{
}

RingBufferPacket::~RingBufferPacket()
{
  if (data)
    delete [] data;
}

RingBuffer::RingBuffer(int capacity)
: m_lock(new Lockable())
, m_capacity(capacity)
, m_count(0)
, m_unread(0)
, m_buffer()
, m_read(nullptr)
, m_write(nullptr)
, m_pool()
{
  assert(capacity > 0);
  m_buffer.resize(capacity);
  init();
}

RingBuffer::~RingBuffer()
{
  OS::CLockGuard g(m_lock->mutex);
  for (std::vector<Chunk*>::iterator it = m_buffer.begin(); it != m_buffer.end(); ++it)
    delete *it;
  while (!m_pool.empty())
  {
    delete m_pool.front();
    m_pool.pop_front();
  }
}

void RingBuffer::init()
{
  Chunk * previous = nullptr;
  for (std::vector<Chunk*>::iterator it = m_buffer.begin(); it != m_buffer.end(); ++it)
  {
    *it = new Chunk();
    if (previous)
      previous->next = *it;
    previous = *it;
  }
  if (m_buffer.begin() != m_buffer.end())
    previous->next = *(m_buffer.begin());
  m_write = *(m_buffer.begin());
  m_read = m_write;
}

int RingBuffer::capacity() const
{
  return m_capacity;
}

int RingBuffer::bytesAvailable() const
{
  OS::CLockGuard g(m_lock->mutex);
  return (m_read != m_write ? m_read->packet->size : 0);
}

unsigned RingBuffer::bytesUnread() const
{
  OS::CLockGuard g(m_lock->mutex);
  return m_unread;
}

bool RingBuffer::full() const
{
  OS::CLockGuard g(m_lock->mutex);
  return (m_unread && m_read == m_write);
}

void RingBuffer::clear()
{
  OS::CLockGuard g(m_lock->mutex);
  m_count = m_unread = 0;
  m_read = m_write;
}

int RingBuffer::write(const char * data, int len)
{
  if (len > 0)
  {
    RingBufferPacket * _packet = needPacket(len);
    _packet->size = len;
    memcpy(_packet->data, data, len);
    {
      OS::CLockGuard g(m_lock->mutex);
      if (m_write->packet)
        freePacket(m_write->packet);
      m_write->packet = _packet;
      m_write->packet->id = ++m_count;
      m_write = m_write->next;
      m_unread += _packet->size;
    }
  }
  return len;
}

RingBufferPacket* RingBuffer::newPacket(int len)
{
  RingBufferPacket * _packet = needPacket(len);
  _packet->size = 0;
  return _packet;
}

void RingBuffer::writePacket(RingBufferPacket* packet)
{
  if (packet)
  {
    OS::CLockGuard g(m_lock->mutex);
    if (m_write->packet)
      freePacket(m_write->packet);
    m_write->packet = packet;
    m_write->packet->id = ++m_count;
    m_write = m_write->next;
    m_unread += packet->size;
  }
}

RingBufferPacket * RingBuffer::read()
{
  RingBufferPacket * p = nullptr;
  {
    OS::CLockGuard g(m_lock->mutex);
    if (m_read != m_write)
    {
      p = m_read->packet;
      m_read->packet = nullptr;
      m_read = m_read->next;
      m_unread -= p->size;
    }
  }
  return p;
}

void RingBuffer::freePacket(RingBufferPacket * p)
{
  m_pool.push_back(p);
}

RingBufferPacket * RingBuffer::needPacket(int size)
{
  RingBufferPacket * p = nullptr;
  if (!m_pool.empty())
  {
    p = m_pool.front();
    m_pool.pop_front();
    if (p->capacity >= size)
    {
      p->id = 0;
      return p;
    }
    //DBG(DBG_DEBUG, "%s: freed packet from buffer (%d)\n", __FUNCTION__, p->capacity);
    delete p;
  }
  p = new RingBufferPacket(size);
  //DBG(DBG_DEBUG, "%s: allocated packet to buffer (%d)\n", __FUNCTION__, p->capacity);
  return p;
}
