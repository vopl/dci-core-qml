/* This file is part of the the dci project. Copyright (C) 2013-2023 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#pragma once

#include "api.hpp"
#include <dci/cmt/future.hpp>
#include <dci/cmt/promise.hpp>

namespace dci::qml
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class To, class From>
    struct FutureCastImpl;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class To, class From>
    struct FutureCastImpl<cmt::Future<To>, cmt::Future<From>>
    {
        static cmt::Future<To> exec(const cmt::Future<From>& from)
        {
            return from.apply() += [](const cmt::Future<From>& in, cmt::Promise<To>& out)
            {
                if(in.resolvedCancel())
                {
                    out.resolveCancel();
                    return;
                }

                if(in.resolvedException())
                {
                    out.resolveException(in.exception());
                    return;
                }

                out.resolve(value_cast<To>(in.value()));
            };
        }
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class To, class From>
    struct FutureCastImpl<cmt::Future<To>, From>
    {
        static cmt::Future<To> exec(const From& from)
        {
            (void)from;
            dbgFatal("not impl");
            return {};
        }
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class To, class From>
    To future_cast(const From& from)
    {
        return FutureCastImpl<To, From>::exec(from);
    }
}

#include "value_cast.hpp"
