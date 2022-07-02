/* This file is part of the the dci project. Copyright (C) 2013-2022 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#pragma once

#include "pch.hpp"

namespace dci::qml
{
    class Handler
        : public QObject
        , public QQmlPropertyValueSource
        , public QQmlParserStatus
    {
        Q_OBJECT
        Q_INTERFACES(QQmlPropertyValueSource)
        Q_INTERFACES(QQmlParserStatus)

        Q_PROPERTY(QVariant target      READ getTarget      WRITE   setTarget   NOTIFY targetChanged)
        Q_PROPERTY(bool     enabled     READ getEnabled     WRITE   setEnabled  NOTIFY enabledChanged)
        Q_PROPERTY(bool     charged     READ getCharged                         NOTIFY chargedChanged)
        Q_PROPERTY(bool     involved    READ getInvolved                        NOTIFY involvedChanged)

    public:
        Handler(QObject* parent=nullptr);
        ~Handler() override;

    private:// QQmlPropertyValueSource
        void setTarget(const QQmlProperty &prop) override;

    private:// QQmlParserStatus
        void classBegin() override;
        void componentComplete() override;

    private:
        QVariant getTarget() const;
        void setTarget(const QVariant& v);
        bool getEnabled() const;
        void setEnabled(bool v);
        bool getCharged() const;
        bool getInvolved() const;

    signals:
        void targetChanged();
        void enabledChanged();
        void chargedChanged();
        void involvedChanged();

    private slots:
        void update();

    private:
        Q_DISABLE_COPY(Handler)

        QQmlProperty    _targetProperty;
        QVariant        _target{};
        bool            _enabled{true};
        bool            _charged{false};
        bool            _involved{false};
        dci::sbs::Owner _connectionsOwner;

        struct Reactor
        {
            QMetaMethod _meta;
            QJSValue    _js;
        };

        QList<Reactor> _reactors;
    };
}
