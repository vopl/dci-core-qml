/* This file is part of the the dci project. Copyright (C) 2013-2022 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#pragma once

#include "qmeta/def.hpp"
#include "qmeta/proxyProvider.hpp"
#include "qmeta/object.hpp"

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
QT_BEGIN_NAMESPACE
    template<typename T>
    requires (dci::qml::qmeta::Def<T>::_declared)
    class QtPrivate::QMetaTypeForType<T>
    {
        using Proxy = typename dci::qml::qmeta::ProxyProvider<T>::Result;

    public:
        static constexpr QMetaTypeInterface::DefaultCtrFn getDefaultCtr()
        {
            if constexpr (std::is_default_constructible_v<Proxy>)
            {
                return [](const QMetaTypeInterface*, void* addr) { new (addr) T{}; };
            }
            else
            {
                return {};
            }
        }

        static constexpr QMetaTypeInterface::CopyCtrFn getCopyCtr()
        {
            if constexpr (std::is_copy_constructible_v<Proxy>)
            {
                return [](const QMetaTypeInterface*, void* addr, const void *other)
                {
                    new (addr) Proxy{*static_cast<const Proxy*>(other)};
                };
            }
            else
            {
                return {};
            }
        }

        static constexpr QMetaTypeInterface::MoveCtrFn getMoveCtr()
        {
            if constexpr (std::is_move_constructible_v<Proxy>)
            {
                return [](const QMetaTypeInterface* , void* addr, void* other)
                {
                    new (addr) Proxy{std::move(*static_cast<Proxy*>(other))};
                };
            }
            else
            {
                return {};
            }
        }

        static constexpr QMetaTypeInterface::DtorFn getDtor()
        {
            return [](const QMetaTypeInterface *, void *addr)
            {
                static_cast<Proxy*>(addr)->~Proxy();
            };
        }

        static constexpr QMetaTypeInterface::LegacyRegisterOp getLegacyRegister()
        {
            return nullptr;
            //return []() { QMetaTypeId2<T>::qt_metatype_id(); };
            //return []() { qRegisterNormalizedMetaType<T>(getName()); };
        }

        static constexpr const char *getName()
        {
            //return dci::qml::qmeta::Def<T>::_name._buf;

            //лучше взять из Stringdata, чтобы не дублировать строку в бинарь
            constexpr uint idx = dci::qml::qmeta::Stringdata<T>::template _indexFor<dci::qml::qmeta::Def<T>::_name>;
            return dci::qml::qmeta::Stringdata<T>::getStr(idx);
        }
    };
QT_END_NAMESPACE

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
QT_BEGIN_NAMESPACE
    template<class T>
    requires (dci::qml::qmeta::Def<T>::_declared)
    struct QtPrivate::MetaObjectForType<T, void>
    {
        static const QMetaObject* value()
        {
            return dci::qml::qmeta::Object<T>::qt();
        }

        static const QMetaObject* metaObjectFunction(const QMetaTypeInterface*)
        {
            return value();
        }
    };
QT_END_NAMESPACE

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
QT_BEGIN_NAMESPACE
    template<class T>
    requires (dci::qml::qmeta::Def<T>::_declared)
    struct QtPrivate::QMetaTypeTypeFlags<T>
    {
        enum
        {
            Flags = QMetaType::NeedsConstruction |
                    QMetaType::NeedsDestruction |
                    QMetaType::IsGadget
        };
    };
QT_END_NAMESPACE

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
QT_BEGIN_NAMESPACE
    template <typename T>
    requires (dci::qml::qmeta::Def<T>::_declared)
    struct QMetaTypeId<T>
    {
        enum { Defined = 1 };
        static int qt_metatype_id()
        {
            return QMetaType::fromType<T>().id();
        }
    };
QT_END_NAMESPACE
