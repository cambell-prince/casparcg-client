#pragma once

#include "../Shared.h"
#include "ui_InspectorChromaWidget.h"

#include "Commands/ChromaCommand.h"
#include "Models/LibraryModel.h"

#include <QtCore/QEvent>
#include <QtCore/QObject>

#include <QtGui/QWidget>

class WIDGETS_EXPORT InspectorChromaWidget : public QWidget, Ui::InspectorChromaWidget
{
    Q_OBJECT

    public:
        explicit InspectorChromaWidget(QWidget* parent = 0);

    protected:
        virtual bool eventFilter(QObject* target, QEvent* event);

    private:
        bool preview;
        LibraryModel* model;
        ChromaCommand* command;

        void loadChroma();


        Q_SLOT void setBlur(int);
        Q_SLOT void setKey(QString);
        Q_SLOT void setShowMask(bool);
        Q_SLOT void setSoftness(int);
        Q_SLOT void setSpill(int);
        Q_SLOT void setThreshold(int);


        Q_SLOT void resetBlur();
        Q_SLOT void resetKey();
        Q_SLOT void resetSoftness();
        Q_SLOT void resetSpill();
        Q_SLOT void resetThreshold();
};
