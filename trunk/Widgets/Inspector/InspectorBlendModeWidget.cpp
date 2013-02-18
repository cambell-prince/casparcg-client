#include "InspectorBlendModeWidget.h"

#include "Global.h"

#include "DatabaseManager.h"
#include "Events/RundownItemPreviewEvent.h"
#include "Events/RundownItemSelectedEvent.h"
#include "Models/BlendModeModel.h"

#include <QtGui/QApplication>

InspectorBlendModeWidget::InspectorBlendModeWidget(QWidget* parent)
    : QWidget(parent), preview(false), model(NULL), command(NULL)
{
    setupUi(this);

    loadBlendMode();
    loadChroma();

    qApp->installEventFilter(this);
}

bool InspectorBlendModeWidget::eventFilter(QObject* target, QEvent* event)
{
    if (event->type() == static_cast<QEvent::Type>(Enum::EventType::RundownItemSelected))
    {
        RundownItemSelectedEvent* rundownItemSelectedEvent = dynamic_cast<RundownItemSelectedEvent*>(event);
        if (dynamic_cast<BlendModeCommand*>(rundownItemSelectedEvent->getCommand()))
        {
            this->preview = false;

            this->model = rundownItemSelectedEvent->getLibraryModel();
            this->command = dynamic_cast<BlendModeCommand*>(rundownItemSelectedEvent->getCommand());

            this->comboBoxBlendMode->setCurrentIndex(this->comboBoxBlendMode->findText(this->command->getBlendMode()));
            this->comboBoxChromaKey->setCurrentIndex(this->comboBoxChromaKey->findText(this->command->getChroma().getKey()));
            this->sliderChromaBlendStart->setValue(QString("%1").arg(this->command->getChroma().getBlendStart() * 100).toFloat());
            this->sliderChromaBlendStop->setValue(QString("%1").arg(this->command->getChroma().getBlendStop() * 100).toFloat());
            this->sliderChromaSpill->setValue(QString("%1").arg(this->command->getChroma().getSpill() * 100).toFloat());

            this->preview = true;
        }
    }

    return QObject::eventFilter(target, event);
}

void InspectorBlendModeWidget::loadBlendMode()
{
    // We do not have a command object, block the signals.
    // Events will not be triggered while we update the values.
    this->comboBoxBlendMode->blockSignals(true);

    QList<BlendModeModel> models = DatabaseManager::getInstance().getBlendMode();
    foreach (BlendModeModel model, models)
        this->comboBoxBlendMode->addItem(model.getValue());

    this->comboBoxBlendMode->blockSignals(false);
}

void InspectorBlendModeWidget::loadChroma()
{
    // We do not have a command object, block the signals.
    // Events will not be triggered while we update the values.
    this->comboBoxChromaKey->blockSignals(true);

    QList<ChromaModel> models = DatabaseManager::getInstance().getChroma();
    foreach (ChromaModel model, models)
        this->comboBoxChromaKey->addItem(model.getKey());

    this->comboBoxChromaKey->blockSignals(false);
}

void InspectorBlendModeWidget::blendModeChanged(QString blendMode)
{
    this->command->setBlendMode(blendMode);

    if (this->preview)
        qApp->postEvent(qApp, new RundownItemPreviewEvent());
}

void InspectorBlendModeWidget::chromaKeyChanged(QString chromaKey)
{
    this->command->getChroma().setKey(chromaKey);

    if (this->preview)
        qApp->postEvent(qApp, new RundownItemPreviewEvent());
}

void InspectorBlendModeWidget::sliderChromaBlendStartChanged(int chromaBlendStart)
{
    this->command->getChroma().setBlendStart(static_cast<float>(chromaBlendStart) / 100);

    this->spinBoxChromaBlendStart->setValue(chromaBlendStart);

    if (this->preview)
        qApp->postEvent(qApp, new RundownItemPreviewEvent());
}

void InspectorBlendModeWidget::spinBoxChromaBlendStartChanged(int chromaBlendStart)
{
    this->sliderChromaBlendStart->setValue(chromaBlendStart);
}

void InspectorBlendModeWidget::sliderChromaBlendStopChanged(int chromaBlendStop)
{
    this->command->getChroma().setBlendStop(static_cast<float>(chromaBlendStop) / 100);

    this->spinBoxChromaBlendStop->setValue(chromaBlendStop);

    if (this->preview)
        qApp->postEvent(qApp, new RundownItemPreviewEvent());
}

void InspectorBlendModeWidget::spinBoxChromaBlendStopChanged(int chromaBlendStop)
{
    this->sliderChromaBlendStop->setValue(chromaBlendStop);
}

void InspectorBlendModeWidget::sliderChromaSpillChanged(int chromaSpill)
{
    this->command->getChroma().setSpill(static_cast<float>(chromaSpill) / 100);

    this->spinBoxChromaSpill->setValue(chromaSpill);

    if (this->preview)
        qApp->postEvent(qApp, new RundownItemPreviewEvent());
}

void InspectorBlendModeWidget::spinBoxChromaSpillChanged(int chromaSpill)
{
    this->sliderChromaSpill->setValue(chromaSpill);
}

void InspectorBlendModeWidget::resetBlendMode(QString blendMode)
{
    this->comboBoxBlendMode->setCurrentIndex(this->comboBoxBlendMode->findText(Mixer::DEFAULT_BLENDMODE));
    this->command->setBlendMode(this->comboBoxBlendMode->currentText());

    if (this->preview)
        qApp->postEvent(qApp, new RundownItemPreviewEvent());
}
void InspectorBlendModeWidget::resetChromaKey(QString chromaMode)
{
    this->comboBoxChromaKey->setCurrentIndex(this->comboBoxChromaKey->findText(Mixer::DEFAULT_CHROMAKEY));
    this->command->getChroma().setKey(this->comboBoxChromaKey->currentText());

    if (this->preview)
        qApp->postEvent(qApp, new RundownItemPreviewEvent());
}

void InspectorBlendModeWidget::resetChromaBlendStart(QString chromaBlendStart)
{
    this->sliderChromaBlendStart->setValue(Mixer::DEFAULT_CHROMABLENDSTART * 100);
    this->command->getChroma().setBlendStart(static_cast<float>(this->sliderChromaBlendStart->value() / 100.0));

    if (this->preview)
        qApp->postEvent(qApp, new RundownItemPreviewEvent());
}

void InspectorBlendModeWidget::resetChromaBlendStop(QString chromaBlendStart)
{
    this->sliderChromaBlendStop->setValue(Mixer::DEFAULT_CHROMABLENDSTOP * 100);
    this->command->getChroma().setBlendStop(static_cast<float>(this->sliderChromaBlendStop->value() / 100.0));

    if (this->preview)
        qApp->postEvent(qApp, new RundownItemPreviewEvent());
}

void InspectorBlendModeWidget::resetChromaSpill(QString chromaBlendStart)
{
    this->sliderChromaSpill->setValue(Mixer::DEFAULT_CHROMASPILL * 100);
    this->command->getChroma().setSpill(static_cast<float>(this->sliderChromaSpill->value() / 100.0));

    if (this->preview)
        qApp->postEvent(qApp, new RundownItemPreviewEvent());
}

