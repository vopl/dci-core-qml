/* This file is part of the the dci project. Copyright (C) 2013-2021 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#pragma once

#include "pch.hpp"

namespace dci::qml
{
    class EventDispatcher;
}

namespace dci::qml::eventDispatcher
{
    class Sockets
    {
    public:
        Sockets(EventDispatcher* ed);
        ~Sockets();

    public:
        void registerSocketNotifier(QSocketNotifier* notifier);
        void unregisterSocketNotifier(QSocketNotifier* notifier);

    public:
        bool sendEvents();

    public:
        void startingUp();
        void closingDown();

    private:
        void ready(QSocketNotifier* qsn);

    private:
        EventDispatcher* _ed{};
        struct State
        {
            Sockets *                           _ss;
            poll::Descriptor                    _descriptor;
            poll::descriptor::ReadyStateFlags   _readyState {};

            std::set<QSocketNotifier *> _notifiers;

            State(Sockets* ss, poll::descriptor::Native native);
            void callback(poll::descriptor::Native native, poll::descriptor::ReadyStateFlags readyState);
        };

        using States = std::map<poll::descriptor::Native, State>;
        States _states;

        std::set<QSocketNotifier*>   _readyNotifiers;
    };
}
