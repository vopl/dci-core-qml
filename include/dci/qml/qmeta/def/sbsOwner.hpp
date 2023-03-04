/* This file is part of the the dci project. Copyright (C) 2013-2023 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#pragma once

#include "../def.hpp"
#include "../api/meth.hpp"
#include <dci/sbs/owner.hpp>
#include <QMetaType>

namespace dci::qml::qmeta
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    struct Def<sbs::Owner>
    {
        using T = sbs::Owner;
        static constexpr bool _declared = true;
        static constexpr Name _name {utils::tname<T>};

        using Api = TList
        <
            api::Meth<"flush", [](T& o) { return o.flush(); }>
        >;
    };
}
