/* This file is part of the the dci project. Copyright (C) 2013-2023 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#pragma once

#include "pch.hpp"
#include "model.hpp"

namespace dci::qml::ep
{
    class Manager;
    struct Extension;
    class Selection
        : public QObject
        , public std::enable_shared_from_this<Selection>
    {
        Q_OBJECT

    public:
        Selection(Manager* m, const QString& tag, std::size_t max);
        ~Selection() override;

        const QString& tag() const;
        std::size_t max() const;

        void detached();

    public:
        void update();

    public:
        QAbstractItemModel* asModel();
        QObjectList asArray();
        QObject* asObject(std::size_t index = 0);

    private:
        Manager* _m;
        QString _tag;
        std::size_t _max;

    private:
        QList<Extension *> _extensions;

    private:
        QObjectList     _objects;
        QPointer<Model> _model;
    };

    using SelectionPtr = std::shared_ptr<Selection>;
}
