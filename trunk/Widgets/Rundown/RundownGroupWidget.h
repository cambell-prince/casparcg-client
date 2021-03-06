#pragma once

#include "../Shared.h"
#include "AbstractRundownWidget.h"
#include "ui_RundownGroupWidget.h"
#include "GpiDevice.h"

#include "Global.h"

#include "Animations/ActiveAnimation.h"
#include "Commands/AbstractCommand.h"
#include "Commands/AbstractPlayoutCommand.h"
#include "Commands/GroupCommand.h"
#include "Models/LibraryModel.h"

#include <stdexcept>

#include <QtCore/QString>

#include <QtGui/QWidget>

class WIDGETS_EXPORT RundownGroupWidget : public QWidget, Ui::RundownGroupWidget, public AbstractRundownWidget, public AbstractPlayoutCommand
{
    Q_OBJECT

    public:
        explicit RundownGroupWidget(const LibraryModel& model, QWidget* parent = 0,
                                    const QString& color = Color::DEFAULT_TRANSPARENT_COLOR,
                                    bool active = false, bool autoStep = false, bool compactView = false);

        virtual AbstractRundownWidget* clone();

        virtual bool isGroup() const;

        virtual AbstractCommand* getCommand();
        virtual LibraryModel* getLibraryModel();

        virtual void setActive(bool active);
        virtual void setInGroup(bool inGroup) {}
        virtual void setColor(const QString& color);

        virtual void setExpanded(bool expanded) {}

        virtual bool executeCommand(enum Playout::PlayoutType::Type type);

        virtual void readProperties(boost::property_tree::wptree& pt);
        virtual void writeProperties(QXmlStreamWriter* writer);

        virtual void setCompactView(bool compactView);

    protected:
        virtual bool eventFilter(QObject* target, QEvent* event);

    private:
        bool active;
        bool autoStep;
        bool compactView;
        QString color;
        LibraryModel model;
        GroupCommand command;
        ActiveAnimation* animation;

        void checkGpiTriggerable();

        Q_SLOT void notesChanged(const QString&);
        Q_SLOT void allowGpiChanged(bool);
        Q_SLOT void autoStepChanged(bool);
        Q_SLOT void gpiDeviceConnected(bool, GpiDevice*);
};
