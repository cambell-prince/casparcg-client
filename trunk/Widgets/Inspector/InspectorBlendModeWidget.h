#pragma once

#include "../Shared.h"
#include "ui_InspectorBlendModeWidget.h"

#include "Commands/BlendModeCommand.h"
#include "Models/LibraryModel.h"

#include <QtCore/QEvent>
#include <QtCore/QObject>

#include <QtGui/QWidget>

class WIDGETS_EXPORT InspectorBlendModeWidget : public QWidget, Ui::InspectorBlendModeWidget
{
    Q_OBJECT

    public:
        explicit InspectorBlendModeWidget(QWidget* parent = 0);

    protected:
        virtual bool eventFilter(QObject* target, QEvent* event);

    private:
        bool preview;
        LibraryModel* model;
        BlendModeCommand* command;

        void loadBlendMode();
        void loadChroma();

        Q_SLOT void blendModeChanged(QString);
        Q_SLOT void chromaKeyChanged(QString);
        Q_SLOT void sliderChromaBlendStartChanged(int);
        Q_SLOT void spinBoxChromaBlendStartChanged(int);
        Q_SLOT void sliderChromaBlendStopChanged(int);
        Q_SLOT void spinBoxChromaBlendStopChanged(int);
        Q_SLOT void sliderChromaSpillChanged(int);
        Q_SLOT void spinBoxChromaSpillChanged(int);


        Q_SLOT void resetBlendMode(QString);
        Q_SLOT void resetChromaKey(QString);
        Q_SLOT void resetChromaBlendStart(QString value);
        Q_SLOT void resetChromaBlendStop(QString value);
        Q_SLOT void resetChromaSpill(QString value);
};
