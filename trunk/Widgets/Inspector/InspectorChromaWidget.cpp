#include "InspectorChromaWidget.h"

#include "Global.h"

#include "DatabaseManager.h"
#include "Events/RundownItemPreviewEvent.h"
#include "Events/RundownItemSelectedEvent.h"
#include "Models/ChromaModel.h"

#include <QtGui/QApplication>

InspectorChromaWidget::InspectorChromaWidget(QWidget* parent)
    : QWidget(parent), preview(false), model(NULL), command(NULL)
{
    setupUi(this);

    loadChroma();

    qApp->installEventFilter(this);
}

bool InspectorChromaWidget::eventFilter(QObject* target, QEvent* event)
{
    if (event->type() == static_cast<QEvent::Type>(Enum::EventType::RundownItemSelected))
    {
        RundownItemSelectedEvent* rundownItemSelectedEvent = dynamic_cast<RundownItemSelectedEvent*>(event);
        if (dynamic_cast<ChromaCommand*>(rundownItemSelectedEvent->getCommand()))
        {
            this->preview = false;

            this->model = rundownItemSelectedEvent->getLibraryModel();
            this->command = dynamic_cast<ChromaCommand*>(rundownItemSelectedEvent->getCommand());

            this->comboBoxKey->setCurrentIndex(this->comboBoxKey->findText(this->command->getKey()));
            this->checkBoxShowMask->setChecked(this->command->getShowMask());
            this->spinBoxThreshold->setValue(this->command->getThreshold()*100);
            this->spinBoxSoftness->setValue(this->command->getSoftness() * 100);
            this->spinBoxBlur->setValue(this->command->getBlur() * 100);
            this->spinBoxSpill->setValue(this->command->getSpill() * 100);

            this->preview = true;
        }
    }

    return QObject::eventFilter(target, event);
}

void InspectorChromaWidget::loadChroma()
{
    // We do not have a command object, block the signals.
    // Events will not be triggered while we update the values.
    this->comboBoxKey->blockSignals(true);

    QList<ChromaModel> models = DatabaseManager::getInstance().getChroma();
    foreach (ChromaModel model, models)
        this->comboBoxKey->addItem(model.getKey());

    this->comboBoxKey->blockSignals(false);
}



void InspectorChromaWidget::setBlur(int blur)
{
    this->command->setBlur(static_cast<float>(blur) / 100.0);
    this->spinBoxBlur->setValue(blur);
    if (this->preview)
        qApp->postEvent(qApp, new RundownItemPreviewEvent());
}

void InspectorChromaWidget::setKey(QString key)
{
    this->command->setKey(key);
    this->comboBoxKey->setCurrentIndex(this->comboBoxKey->findText(key));
    if (this->preview)
        qApp->postEvent(qApp, new RundownItemPreviewEvent());
}

void InspectorChromaWidget::setShowMask(bool show)
{
    this->command->setShowMask(show);
    this->checkBoxShowMask->setChecked(show);
    if (this->preview)
        qApp->postEvent(qApp, new RundownItemPreviewEvent());
}

void InspectorChromaWidget::setSoftness(int softness)
{
    this->command->setSpread(static_cast<float>(softness) / 100.0);
    this->spinBoxSoftness->setValue(softness);
    if (this->preview)
        qApp->postEvent(qApp, new RundownItemPreviewEvent());
}

void InspectorChromaWidget::setSpill(int spill)
{
    this->command->setSpill(static_cast<float>(spill) / 100.0);
    this->spinBoxSpill->setValue(spill);
    if (this->preview)
        qApp->postEvent(qApp, new RundownItemPreviewEvent());
}

void InspectorChromaWidget::setThreshold(int threshold)
{
    this->command->setThreshold(static_cast<float>(threshold) / 100.0);
    this->spinBoxThreshold->setValue(threshold);
    if (this->preview)
        qApp->postEvent(qApp, new RundownItemPreviewEvent());
}


void InspectorChromaWidget::resetBlur()
{
    setBlur(Mixer::DEFAULT_CHROMABLUR);
}

void InspectorChromaWidget::resetKey()
{
    setKey(Mixer::DEFAULT_CHROMAKEY);
}

void InspectorChromaWidget::resetThreshold()
{
    setThreshold(Mixer::DEFAULT_CHROMABLENDSTART * 100);
}

void InspectorChromaWidget::resetSoftness()
{
    setSoftness(Mixer::DEFAULT_CHROMABLENDSTOP * 100);
}

void InspectorChromaWidget::resetSpill()
{
    setSpill(Mixer::DEFAULT_CHROMASPILL * 100);
}
