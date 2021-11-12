/* This file is part of the the dci project. Copyright (C) 2013-2021 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include "pch.hpp"
#include "app.hpp"
#include <dci/qml/app.hpp>
#include "../eventDispatcher.hpp"
#include "../ep/manager.hpp"
#include "../handler.hpp"

namespace dci::qml::impl
{
    namespace
    {
        int fakeArgc = 0;

        const char bootstrapperCode[] = R"(
import QtQml
import dci as Dci
Instantiator {
    model: Dci.Hook.allAsModel("entry");
    delegate: Instantiator {
        delegate: modelData
    }
})";

        void qtMsgHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
        {
            QStringList fullMsgParts;

            if(context.category)    fullMsgParts << QString{"qt["}+context.category+"]";
            else                    fullMsgParts << QString{"qt"};

            fullMsgParts << msg;

            if(context.file)        fullMsgParts << QString{"at "} + context.file + ":" + QString::number(context.line);
            if(context.function)    fullMsgParts << QString{"fn "} + context.function;

            std::string fullMsg = fullMsgParts.join(", ").toStdString();

            switch(type)
            {
            case QtDebugMsg:
                LOGD(fullMsg);
                break;
            case QtWarningMsg:
                LOGW(fullMsg);
                break;
            //case QtSystemMsg: // ==QtCriticalMsg
            case QtCriticalMsg:
                LOGE(fullMsg);
                break;
            case QtFatalMsg:
                LOGF(fullMsg);
                break;
            case QtInfoMsg:
                LOGI(fullMsg);
                break;
            default:
                LOGI(fullMsg);
                break;
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    AppPtr App::instance()
    {
        AppPtr res = _instance.lock();

        if(!res)
        {
            qInstallMessageHandler(qtMsgHandler);
            QApplication::setEventDispatcher(new EventDispatcher);
            QApplication::setQuitOnLastWindowClosed(false);
            res.reset(himpl::impl2Face<qml::App>(new App));
            _instance = res;
        }

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    App::App()
        : _qapp{fakeArgc, nullptr}
        , _qengine{}
        , _hook{}
    {
        _qapp.setFont(QFont{QStringList{} << "Open Sans", 10, QFont::Normal, false});

        cmt::spawn() += _mainLoopOwner * [this]
        {
            while(!_qappStop)
            {
                LOGI("qt app exec");
                QApplication::exec();
                LOGI("qt app done");
            }
        };

        //_qml.globalObject().setProperty("Error", _qml.newQMetaObject(&dci::qml::Error::staticMetaObject));

        qmlRegisterSingletonInstance("dci", 1, 0, "Hook", &_hook);
        qmlRegisterType<Handler>("dci", 1, 0, "Handler");

        ep::g_manager.startup(himpl::impl2Face<qml::App>(this));

        _qengine.loadData(bootstrapperCode, QUrl{"Bootstrapper.qml"});
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    App::~App()
    {
        ep::g_manager.shutdown(himpl::impl2Face<qml::App>(this));

        for(QObject* ro : _qengine.rootObjects())
        {
            ro->deleteLater();
        }

        _qengine.clearComponentCache();
        _qengine.collectGarbage();

        _qappStop = true;
        _qapp.quit();
        _mainLoopOwner.stop();

        _qengine.collectGarbage();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    QApplication* App::qapp()
    {
        return &_qapp;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    QQmlApplicationEngine* App::qengine()
    {
        return &_qengine;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    QObject* App::loadScript(const QString& filePath)
    {
        QObject* res{};

        auto c = QObject::connect(&_qengine, &QQmlApplicationEngine::objectCreated, [&](QObject* object, const QUrl&)
        {
            res = object;
        });

        _qengine.load(filePath);

        QObject::disconnect(c);

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::weak_ptr<qml::App> App::_instance {};
}
