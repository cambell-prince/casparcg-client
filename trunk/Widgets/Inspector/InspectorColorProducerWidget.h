#pragma once

#include "../Shared.h"
#include "ui_InspectorColorProducerWidget.h"

#include "Commands/ColorCommand.h"
#include "Models/LibraryModel.h"

#include <QtCore/QEvent>
#include <QtCore/QObject>
#include <QtCore/QString>

#include <QtGui/QWidget>

class WIDGETS_EXPORT InspectorColorProducerWidget : public QWidget, Ui::InspectorColorProducerWidget
{
    Q_OBJECT

    public:
        explicit InspectorColorProducerWidget(QWidget* parent = 0);

    protected:
        virtual bool eventFilter(QObject* target, QEvent* event);

    private:
        bool preview;
        LibraryModel* model;
        ColorCommand* command;

        void loadDirection();
        void loadTransition();
        void loadTween();

        Q_SLOT void colorDialogClicked();
        Q_SLOT void transitionChanged(QString);
        Q_SLOT void durationChanged(int);
        Q_SLOT void directionChanged(QString);
        Q_SLOT void tweenChanged(QString);
        Q_SLOT void useAutoChanged(int);
        Q_SLOT void resetTransition(QString);
        Q_SLOT void resetDuration(QString);
        Q_SLOT void resetDirection(QString);
        Q_SLOT void resetTween(QString);
        Q_SLOT void resetUseAuto(QString);
};
