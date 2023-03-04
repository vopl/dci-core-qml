/* This file is part of the the dci project. Copyright (C) 2013-2023 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include "pch.hpp"
#include <dci/qml/error.hpp>

namespace dci::qml
{
    inline std::exception_ptr exceptionConvert(const QJSValue& v)
    {
        if(v.isQObject())
        {
            Error* error = qobject_cast<Error*>(v.toQObject());
            if(error)
            {
                std::exception_ptr cppException = error->cppException();

                if(cppException)
                {
                    return cppException;
                }
            }
        }

        return exception::buildInstance<dci::Exception>(v.toString().toStdString());
    }

    inline QJSValue exceptionConvert(QJSEngine* jse, std::exception_ptr e)
    {
        return jse->newQObject(new Error{e});
    }
}
