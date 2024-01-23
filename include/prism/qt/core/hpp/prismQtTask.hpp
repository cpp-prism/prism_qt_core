#ifndef PRISM_QT_CORE_PRISMQTTASK_HPP
#define PRISM_QT_CORE_PRISMQTTASK_HPP

#include <QCoreApplication>
#include <QFuture>
#include <QFutureWatcher>
#include <QSharedPointer>
#include <QThreadPool>
#include <QtConcurrent/QtConcurrent>
#include <iostream>
#include "../prismQt_core_global.h"

namespace prism::qt::core{

template <typename T>
class prismQtTask;

template <typename T>
class  prismQtTaskBase : public QObject, public std::enable_shared_from_this<prismQtTaskBase<T>>
{
  public:
    prismQtTaskBase()
    {
        volatile bool dummy = true;
        Q_UNUSED(dummy)
        //        qDebug() << "构造 task" << this;
    }
    ~prismQtTaskBase()
    {
        volatile bool dummy = true;
        Q_UNUSED(dummy)
        //        qInfo() << "析构 task " << this;
    }
    QFuture<T> futureResult{};
    QFutureWatcher<T> w;
    template <typename F>
    void thenSync(F ptr_run)
    {
        auto self = this->shared_from_this();
        QObject::connect(&w, &QFutureWatcherBase::finished, [=] {
            try
            {
                ptr_run();
            }
            catch (std::exception& ex)
            {
                qDebug() << QString::fromStdString(std::string(ex.what()));
            }
            catch (...)
            {
                qDebug() << "unknow exception";
            }
            self->w.disconnect();
        });
        w.setFuture(futureResult);
    }

    //template <typename F>
    //auto thenAsync(F ptr_run) -> std::enable_if_t<std::is_same_v<decltype(ptr_run()), void>, std::shared_ptr<prismQtTaskBase<void>>>
    //{
    //    std::shared_ptr<prismQtTaskBase<void>> t = std::make_shared<prismQtTaskBase<void>>();
    //    w.setFuture(futureResult);
    //    auto self = this->shared_from_this();
    //    t->futureResult = QtConcurrent::run([self, ptr_run, t] {
    //        while (!self->futureResult.isFinished())
    //            QCoreApplication::processEvents();
    //        try
    //        {
    //            ptr_run();
    //        }
    //        catch (std::exception& ex)
    //        {
    //            qDebug() << QString::fromStdString(std::string(ex.what()));
    //        }
    //        catch (...)
    //        {
    //            qDebug() << "unknow exception";
    //        }
    //        self->w.disconnect();
    //    });
    //    return t;
    //}

    //template <typename F>
    //auto thenAsync(F ptr_run) -> std::enable_if_t<!std::is_same_v<decltype(ptr_run()), void>, std::shared_ptr<prismQtTask<decltype(ptr_run())>>>
    //{
    //    auto self = this->shared_from_this();
    //    std::shared_ptr<prismQtTask<decltype(ptr_run())>> t = std::make_shared<prismQtTask<decltype(ptr_run())>>();
    //    w.setFuture(futureResult);
    //    t->futureResult = QtConcurrent::run([self, ptr_run] {
    //        while (!self->futureResult.isFinished())
    //            QCoreApplication::processEvents();
    //        try
    //        {
    //            return ptr_run();
    //        }
    //        catch (std::exception& ex)
    //        {
    //            qDebug() << QString::fromStdString(std::string(ex.what()));
    //        }
    //        catch (...)
    //        {
    //            qDebug() << "unknow exception";
    //        }
    //        self->w.disconnect();
    //        return decltype(ptr_run())();
    //    });
    //    return t;
    //}
};

template <typename T>
class  prismQtTask : public prismQtTaskBase<T>
{
  public:
    using prismQtTaskBase<T>::futureResult;
    using prismQtTaskBase<T>::w;
    template <typename F>
    void thenSync(F ptr_run)
    {
        auto self = this->shared_from_this();
        QObject::connect(&w, &QFutureWatcherBase::finished, [self, ptr_run] {
            try
            {
                ptr_run(self->w.result());
            }
            catch (std::exception& ex)
            {
                qDebug() << QString::fromStdString(std::string(ex.what()));
            }
            catch (...)
            {
                qDebug() << "unknow exception";
            }
            self->w.disconnect();
        });
        w.setFuture(futureResult);
    }

    //template <typename F>
    //auto thenAsync(F ptr_run) -> std::enable_if_t<std::is_same_v<decltype(ptr_run(T())), void>, std::shared_ptr<prismQtTaskBase<void>>>
    //{
    //    std::shared_ptr<prismQtTaskBase<void>> t = std::make_shared<prismQtTaskBase<void>>();
    //    w.setFuture(futureResult);
    //    auto self = this->shared_from_this();
    //    t->futureResult = QtConcurrent::run([self, ptr_run, t] {
    //        while (!self->futureResult.isFinished())
    //            QCoreApplication::processEvents();
    //        try
    //        {
    //            ptr_run(self->w.result());
    //        }
    //        catch (std::exception& ex)
    //        {
    //            qDebug() << QString::fromStdString(std::string(ex.what()));
    //        }
    //        catch (...)
    //        {
    //            qDebug() << "unknow exception";
    //        }
    //        self->w.disconnect();
    //        return decltype(ptr_run(T()))();
    //    });
    //    return t;
    //}

    //template <typename F>
    //auto thenAsync(F ptr_run) -> std::enable_if_t<!std::is_same_v<decltype(ptr_run(T())), void>, std::shared_ptr<prismQtTask<decltype(ptr_run(T()))>>>
    //{
    //    auto self = this->shared_from_this();
    //    std::shared_ptr<prismQtTask<decltype(ptr_run(T()))>> t = std::make_shared<prismQtTask<decltype(ptr_run(T()))>>();
    //    w.setFuture(futureResult);
    //    t->futureResult = QtConcurrent::run([self, ptr_run] {
    //        while (!self->futureResult.isFinished())
    //            QCoreApplication::processEvents();
    //        try
    //        {
    //            return ptr_run(self->w.result());
    //        }
    //        catch (std::exception& ex)
    //        {
    //            qDebug() << QString::fromStdString(std::string(ex.what()));
    //        }
    //        catch (...)
    //        {
    //            qDebug() << "unknow exception";
    //        }
    //        self->w.disconnect();
    //        return decltype(ptr_run(T()))();
    //    });
    //    return t;
    //}
};
class prismQtTaskManager : public QObject
{
  public:
    template <typename F>
    static auto runAsync(F ptr_run) -> std::enable_if_t<!std::is_same_v<decltype(ptr_run()), void>, std::shared_ptr<prismQtTask<decltype(ptr_run())>>>
    {
        std::shared_ptr<prismQtTask<decltype(ptr_run())>> t = std::make_shared<prismQtTask<decltype(ptr_run())>>();
        QFuture<decltype(ptr_run())> ff;
        t->futureResult = QtConcurrent::run(ptr_run);
        return t;
    }
    template <typename F>
    static auto runAsync(F ptr_run) -> std::enable_if_t<std::is_same_v<decltype(ptr_run()), void>, std::shared_ptr<prismQtTaskBase<void>>>
    {
        std::shared_ptr<prismQtTaskBase<void>> t = std::make_shared<prismQtTaskBase<void>>();
        t->futureResult = QtConcurrent::run(ptr_run);
        return t;
    }
};


}
#endif // PRISM_QT_CORE_PRISMQTTASK_HPP
