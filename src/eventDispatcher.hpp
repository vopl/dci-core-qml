/* This file is part of the the dci project. Copyright (C) 2013-2022 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#pragma once

#include "pch.hpp"
#include "eventDispatcher/sockets.hpp"
#include "eventDispatcher/timers.hpp"

namespace dci::qml
{
    class EventDispatcher
        : public QAbstractEventDispatcher
    {
        Q_OBJECT

    public:
        EventDispatcher();
        ~EventDispatcher() override;

    public:
        void socketsReady();
        void timersReady();

    private:
        bool processEvents(QEventLoop::ProcessEventsFlags flags) override;

    private:
        void registerSocketNotifier(QSocketNotifier* notifier) override;
        void unregisterSocketNotifier(QSocketNotifier* notifier) override;

    private:
        void registerTimer(int timerId, qint64 interval, Qt::TimerType timerType, QObject* object) override;
        bool unregisterTimer(int timerId) override;
        bool unregisterTimers(QObject* object) override;
        QList<TimerInfo> registeredTimers(QObject* object) const override;

        int remainingTime(int timerId) override;

    private:
        void wakeUp() override;
        void interrupt() override;

    private:
        void startingUp() override;
        void closingDown() override;

    private:
        std::atomic_bool    _interrupt{};
        cmt::Notifier       _readyNotifier;
        sbs::Owner          _awakeOwner;
        poll::Awaker        _awaker;

    private:
        eventDispatcher::Sockets    _sockets;
        eventDispatcher::Timers     _timers;
    };
}
