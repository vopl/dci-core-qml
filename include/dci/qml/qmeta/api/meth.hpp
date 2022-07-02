/* This file is part of the the dci project. Copyright (C) 2013-2022 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#pragma once

#include "../common.hpp"

namespace dci::qml::qmeta::api
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <Name name, auto invoke, Name... paramNames>
    requires
    (
        CallableSignature<decltype(invoke)>::_valid &&
        sizeof...(paramNames)+1 == CallableSignature<decltype(invoke)>::ParamList::_size
    )
    struct Meth
    {
        static constexpr bool       _isMeth {true};
        static constexpr auto       _name {name};
        static constexpr Name<0>    _tag {""};
        static constexpr auto       _invoke {invoke}; // Return (Gadget&, ParamType... paramName)

        using InvokeSignature   = CallableSignature<decltype(invoke)>;
        using ParamTypes        = typename InvokeSignature::ParamList::template PopFront<>::template Map<std::decay_t>;
        using ParamNames        = VList<paramNames...>;
        using Return            = typename InvokeSignature::Return;
        using Strings           = typename VList<_name, _tag>::template Append<ParamNames>;
        using MetaTypes         = typename ParamTypes::template PushFront<Return>;
    };

    template <class T, class=void> struct IsMethImpl: Value<false>{};
    template <class T> struct IsMethImpl<T, std::void_t<decltype(T::_isMeth)>>: Value<T::_isMeth>{};
    template <class T> using IsMeth = Value<IsMethImpl<T>::_v>;
}
