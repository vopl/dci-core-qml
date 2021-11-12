/* This file is part of the the dci project. Copyright (C) 2013-2021 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include "pch.hpp"
#include "sockets.hpp"
#include "../eventDispatcher.hpp"

namespace dci::qml::eventDispatcher
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Sockets::Sockets(EventDispatcher* ed)
        : _ed{ed}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Sockets::~Sockets()
    {
        closingDown();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Sockets::registerSocketNotifier(QSocketNotifier* notifier)
    {
        int fd = static_cast<int>(notifier->socket());
        State& s = _states.try_emplace(
                       fd,
                       this, fd).first->second;

        s._notifiers.insert(notifier);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Sockets::unregisterSocketNotifier(QSocketNotifier* notifier)
    {
        int fd = static_cast<int>(notifier->socket());

        auto iter = _states.find(fd);
        if(_states.end() == iter)
        {
            return;
        }

        State& s = iter->second;
        s._notifiers.erase(notifier);
        if(s._notifiers.empty())
        {
            s._descriptor.detach();
            _states.erase(iter);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Sockets::sendEvents()
    {
        bool res = false;

        {
            QEvent qe{QEvent::SockAct};

            auto iter{_readyNotifiers.begin()};
            while(iter!=_readyNotifiers.end())
            {
                QSocketNotifier* snp = *iter;
                iter = _readyNotifiers.erase(iter);

                if(snp->isEnabled())
                {
                    QApplication::sendEvent(snp, &qe);
                    res = true;
                }
            }
        }

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Sockets::startingUp()
    {
        //empty
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Sockets::closingDown()
    {
        for(auto &[fd, s] : _states)
        {
            s._descriptor.detach();
            s._notifiers.clear();
        }

        _states.clear();
        _readyNotifiers.clear();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Sockets::ready(QSocketNotifier* qsn)
    {
        _readyNotifiers.emplace(qsn);
        _ed->socketsReady();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Sockets::State::State(Sockets* ss, int fd)
        : _ss{ss}
        , _descriptor{fd, [this](int fd, std::uint_fast32_t readyState){callback(fd, readyState);}}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Sockets::State::callback(int fd, std::uint_fast32_t flags)
    {
        (void)fd;

        dbgAssert(_descriptor.fd() == fd);

        _flags |= flags;

        std::uint_fast32_t errMask = poll::Descriptor::rsf_error | poll::Descriptor::rsf_close | poll::Descriptor::rsf_eof;

        for(QSocketNotifier* qsn : _notifiers)
        {
            switch(qsn->type())
            {
            case QSocketNotifier::Read:
                if(_flags & (errMask | poll::Descriptor::rsf_read))
                {
                    _flags &= ~poll::Descriptor::rsf_read;
                    _ss->ready(qsn);
                }
                break;

            case QSocketNotifier::Write:
                if(_flags & (errMask | poll::Descriptor::rsf_write))
                {
                    _flags &= ~poll::Descriptor::rsf_write;
                    _ss->ready(qsn);
                }
                break;

            case QSocketNotifier::Exception:
                if(_flags & (errMask | poll::Descriptor::rsf_pri))
                {
                    _flags &= ~poll::Descriptor::rsf_pri;
                    _ss->ready(qsn);
                }
                break;

            default:
                break;
            }
        }
    }
}
