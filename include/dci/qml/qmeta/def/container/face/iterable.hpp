/* This file is part of the the dci project. Copyright (C) 2013-2021 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#pragma once

#include "../../../common.hpp"
#include "../../../api/prop.hpp"
#include "../../../api/meth.hpp"
#include "../iterator.hpp"
#include "../../../../app.hpp"
#include <dci/utils/intrusiveDlist.hpp>
#include <QJSValue>
#include <QQmlApplicationEngine>
//#include <QJSEngine>

namespace dci::qml::qmeta::def::container::face
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    struct Iterable
    {
        template <class Assembly>
        using Api = TList
        <
//            Array.prototype.forEach()
            api::Meth<"forEach", [](T& /*o*/, const QJSValue& /*callbackFn*/) { /*return Assembly::forEach(o, callbackFn);*/ }, "callbackFn">,
            api::Meth<"forEach", [](T& /*o*/, const QJSValue& /*callbackFn*/, const QJSValue& /*thisArg*/) { /*return Assembly::forEach(o, callbackFn, thisArg);*/ }, "callbackFn", "thisArg">,

//            Array.prototype.map()
            api::Meth<"map", [](T& /*o*/, const QJSValue& /*callbackFn*/) { /*return Assembly::forEach(o, callbackFn);*/ }, "callbackFn">,
            api::Meth<"map", [](T& /*o*/, const QJSValue& /*callbackFn*/, const QJSValue& /*thisArg*/) { /*return Assembly::forEach(o, callbackFn, thisArg);*/ }, "callbackFn", "thisArg">,

//            Array.prototype.reduce()
//            Array.prototype.reduceRight()
//            Array.prototype.join()
//            Array.prototype.every()
//            Array.prototype.some()


            api::Meth<"", [](const T& /*o*/){return 0;}>
        >;

    public:
        template <class Assembly>
        static void forEach(Assembly& o, const QJSValue& cb,  const QJSValue& instance = {});

        template <class Assembly>
        static void map(Assembly& o, const QJSValue& cb,  const QJSValue& instance = {});

    public:
        Iterable();
        Iterable(const Iterable&);
        Iterable(Iterable&&);
        ~Iterable();

        Iterable& operator=(const Iterable&);
        Iterable& operator=(Iterable&&);

    private:
        friend class Iterator<T>;
        void addIterator(Iterator<T>*);
        void delIterator(Iterator<T>*);
        void resetIterators();

    private:
        utils::IntrusiveDlist<Iterator<T>> _iterators{};
        static_assert (8*4 == sizeof(_iterators));
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    template <class Assembly>
    void Iterable<T>::forEach(Assembly& o, const QJSValue& cb,  const QJSValue& instance)
    {
        Iterator<T> iter{&o};
        QJSValueList args{2};
        while(iter.nextImpl())
        {
            args[0] = iter.value();
            args[1] = QJSValue{static_cast<int>(iter.key())};
            QJSValue res = cb.callWithInstance(instance, args);
            if(res.isError())
            {
                App::instance()->qengine()->throwError(res);
                break;
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    template <class Assembly>
    void Iterable<T>::map(Assembly& o, const QJSValue& cb,  const QJSValue& instance)
    {
        T res;

        Iterator<T> iter{&o};
        QJSValueList args{2};
        while(iter.nextImpl())
        {
            args[0] = iter.value();
            args[1] = QJSValue{static_cast<int>(iter.key())};
            QJSValue mapped = cb.callWithInstance(instance, args);
            if(mapped.isError())
            {
                App::instance()->qengine()->throwError(res);
                break;
            }

            res.push_back(qjsvalue_cast<T::value_type>(mapped));
        }

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    Iterable<T>::Iterable()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    Iterable<T>::Iterable(const Iterable<T>&)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    Iterable<T>::Iterable(Iterable<T>&& from)
    {
        from.resetIterators();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    Iterable<T>::~Iterable()
    {
        resetIterators();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    Iterable<T>& Iterable<T>::operator=(const Iterable<T>& from)
    {
        if(this != &from)
        {
            resetIterators();
        }

        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    Iterable<T>& Iterable<T>::operator=(Iterable<T>&& from)
    {
        if(this != &from)
        {
            resetIterators();
            from.resetIterators();
        }

        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void Iterable<T>::addIterator(Iterator<T>* iterator)
    {
        _iterators.push(iterator);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void Iterable<T>::delIterator(Iterator<T>* iterator)
    {
        _iterators.remove(iterator);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void Iterable<T>::resetIterators()
    {
        while(!_iterators.empty())
        {
            _iterators.first()->reset();// сам вырежет себя из контейнера
        }
    }
}
