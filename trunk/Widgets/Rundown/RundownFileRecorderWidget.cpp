#include "RundownFileRecorderWidget.h"

#include "Global.h"

#include "DeviceManager.h"
#include "GpiManager.h"
#include "Events/ConnectionStateChangedEvent.h"
#include "Events/RundownItemChangedEvent.h"

#include <QtCore/QObject>
#include <QtCore/QTimer>

RundownFileRecorderWidget::RundownFileRecorderWidget(const LibraryModel& model, QWidget* parent, const QString& color,
                                                     bool active, bool inGroup, bool disconnected)
    : QWidget(parent),
      active(active), inGroup(inGroup), disconnected(disconnected), color(color), model(model)
{
    setupUi(this);

    setActive(active);

    this->labelDisconnected->setVisible(this->disconnected);
    this->labelGroupColor->setVisible(this->inGroup);
    this->labelGroupColor->setStyleSheet(QString("background-color: %1;").arg(Color::DEFAULT_GROUP_COLOR));
    this->labelColor->setStyleSheet(QString("background-color: %1;").arg(color));

    this->labelName->setText(this->model.getName());
    this->labelChannel->setText(QString("Channel: %1").arg(this->command.getChannel()));
    this->labelVideolayer->setText(QString("Video layer: %1").arg(this->command.getVideolayer()));
    this->labelDelay->setText(QString("Delay: %1").arg(this->command.getDelay()));
    this->labelDevice->setText(QString("Device: %1").arg(this->model.getDeviceName()));

    QObject::connect(&this->command, SIGNAL(channelChanged(int)), this, SLOT(channelChanged(int)));
    QObject::connect(&this->command, SIGNAL(videolayerChanged(int)), this, SLOT(videolayerChanged(int)));
    QObject::connect(&this->command, SIGNAL(delayChanged(int)), this, SLOT(delayChanged(int)));
    QObject::connect(&this->command, SIGNAL(allowGpiChanged(bool)), this, SLOT(allowGpiChanged(bool)));
    QObject::connect(GpiManager::getInstance().getGpiDevice().data(), SIGNAL(connectionStateChanged(bool, GpiDevice*)),
                     this, SLOT(gpiDeviceConnected(bool, GpiDevice*)));

    checkEmptyDevice();
    checkGpiTriggerable();

    qApp->installEventFilter(this);
}

bool RundownFileRecorderWidget::eventFilter(QObject* target, QEvent* event)
{
    if (event->type() == static_cast<QEvent::Type>(Enum::EventType::RundownItemChanged))
    {
        // This event is not for us.
        if (!this->active)
            return false;

        RundownItemChangedEvent* rundownItemChangedEvent = dynamic_cast<RundownItemChangedEvent*>(event);
        this->model.setName(rundownItemChangedEvent->getName());
        this->model.setDeviceName(rundownItemChangedEvent->getDeviceName());

        this->labelName->setText(this->model.getName());
        this->labelChannel->setText(QString("Channel: %1").arg(this->command.getChannel()));
        this->labelVideolayer->setText(QString("Videolayer: %1").arg(this->command.getVideolayer()));
        this->labelDelay->setText(QString("Delay: %1").arg(this->command.getDelay()));
        this->labelDevice->setText(QString("Device: %1").arg(this->model.getDeviceName()));

        checkEmptyDevice();
    }
    else if (event->type() == static_cast<QEvent::Type>(Enum::EventType::RundownItemPreview))
    {
        // This event is not for us.
        if (!this->active)
            return false;

        executePlay();
    }
    else if (event->type() == static_cast<QEvent::Type>(Enum::EventType::ConnectionStateChanged))
    {
        ConnectionStateChangedEvent* connectionStateChangedEvent = dynamic_cast<ConnectionStateChangedEvent*>(event);
        if (connectionStateChangedEvent->getDeviceName() == this->model.getDeviceName())
        {
            this->disconnected = !connectionStateChangedEvent->getConnected();

            if (connectionStateChangedEvent->getConnected())
                this->labelDisconnected->setVisible(false);
            else
                this->labelDisconnected->setVisible(true);
        }
    }

    return QObject::eventFilter(target, event);
}

IRundownWidget* RundownFileRecorderWidget::clone()
{
    RundownFileRecorderWidget* widget = new RundownFileRecorderWidget(this->model, this->parentWidget(), this->color,
                                                                      this->active, this->inGroup, this->disconnected);

    FileRecorderCommand* command = dynamic_cast<FileRecorderCommand*>(widget->getCommand());
    command->setChannel(this->command.getChannel());
    command->setVideolayer(this->command.getVideolayer());
    command->setDelay(this->command.getDelay());
    command->setAllowGpi(this->command.getAllowGpi());
    command->setFilename(this->command.getFilename());
    command->setContainer(this->command.getContainer());
    command->setCodec(this->command.getCodec());
    command->setPreset(this->command.getPreset());
    command->setTune(this->command.getTune());

    return widget;
}

bool RundownFileRecorderWidget::isGroup() const
{
    return false;
}

ICommand* RundownFileRecorderWidget::getCommand()
{
    return &this->command;
}

LibraryModel* RundownFileRecorderWidget::getLibraryModel()
{
    return &this->model;
}

void RundownFileRecorderWidget::setActive(bool active)
{
    this->active = active;

    if (this->active)
        this->labelActiveColor->setStyleSheet("background-color: red;");
    else
        this->labelActiveColor->setStyleSheet("");
}

void RundownFileRecorderWidget::setInGroup(bool inGroup)
{
    this->inGroup = inGroup;
    this->labelGroupColor->setVisible(inGroup);

    if (this->inGroup)
    {
        this->labelChannel->setGeometry(this->labelChannel->geometry().x() + Define::GROUP_XPOS_OFFSET,
                                        this->labelChannel->geometry().y(),
                                        this->labelChannel->geometry().width(),
                                        this->labelChannel->geometry().height());
        this->labelVideolayer->setGeometry(this->labelVideolayer->geometry().x() + Define::GROUP_XPOS_OFFSET,
                                           this->labelVideolayer->geometry().y(),
                                           this->labelVideolayer->geometry().width(),
                                           this->labelVideolayer->geometry().height());
        this->labelDelay->setGeometry(this->labelDelay->geometry().x() + Define::GROUP_XPOS_OFFSET,
                                      this->labelDelay->geometry().y(),
                                      this->labelDelay->geometry().width(),
                                      this->labelDelay->geometry().height());
        this->labelDevice->setGeometry(this->labelDevice->geometry().x() + Define::GROUP_XPOS_OFFSET,
                                       this->labelDevice->geometry().y(),
                                       this->labelDevice->geometry().width(),
                                       this->labelDevice->geometry().height());
    }
}

void RundownFileRecorderWidget::setColor(const QString& color)
{
    this->color = color;
    this->labelColor->setStyleSheet(QString("background-color: %1;").arg(color));
}

void RundownFileRecorderWidget::checkEmptyDevice()
{
    if (this->labelDevice->text() == "Device: ")
        this->labelDevice->setStyleSheet("color: black;");
    else
        this->labelDevice->setStyleSheet("");
}

bool RundownFileRecorderWidget::executeCommand(enum Playout::PlayoutType::Type type)
{
    if (type == Playout::PlayoutType::Stop)
        QTimer::singleShot(0, this, SLOT(executeStop()));
    else if (type == Playout::PlayoutType::Play)
        QTimer::singleShot(this->command.getDelay(), this, SLOT(executePlay()));

    return true;
}

void RundownFileRecorderWidget::executeStop()
{
    const QSharedPointer<CasparDevice> device = DeviceManager::getInstance().getConnectionByName(this->model.getDeviceName());
    if (device != NULL && device->isConnected())
        device->stopRecording(this->command.getChannel());

    foreach (const DeviceModel& model, DeviceManager::getInstance().getDeviceModels())
    {
        if (model.getShadow() == "No")
            continue;

        const QSharedPointer<CasparDevice> deviceShadow = DeviceManager::getInstance().getConnectionByName(model.getName());
        if (deviceShadow != NULL && deviceShadow->isConnected())
            deviceShadow->stopRecording(this->command.getChannel());
    }
}

void RundownFileRecorderWidget::executePlay()
{
    const QSharedPointer<CasparDevice> device = DeviceManager::getInstance().getConnectionByName(this->model.getDeviceName());
    if (device != NULL && device->isConnected())
        device->startRecording(this->command.getChannel(), this->command.getFilename(), this->command.getCodec());

    foreach (const DeviceModel& model, DeviceManager::getInstance().getDeviceModels())
    {
        if (model.getShadow() == "No")
            continue;

        const QSharedPointer<CasparDevice>  deviceShadow = DeviceManager::getInstance().getConnectionByName(model.getName());
        if (deviceShadow != NULL && deviceShadow->isConnected())
            deviceShadow->startRecording(this->command.getChannel(), this->command.getFilename(), this->command.getCodec());
    }
}

void RundownFileRecorderWidget::channelChanged(int channel)
{
    this->labelChannel->setText(QString("Channel: %1").arg(channel));
}

void RundownFileRecorderWidget::videolayerChanged(int videolayer)
{
    this->labelVideolayer->setText(QString("Video layer: %1").arg(videolayer));
}

void RundownFileRecorderWidget::delayChanged(int delay)
{
    this->labelDelay->setText(QString("Delay: %1").arg(delay));
}

void RundownFileRecorderWidget::checkGpiTriggerable()
{
    labelGpiTriggerable->setVisible(this->command.getAllowGpi());

    if (GpiManager::getInstance().getGpiDevice()->isConnected())
        labelGpiTriggerable->setPixmap(QPixmap(":/Graphics/Images/GpiConnected.png"));
    else
        labelGpiTriggerable->setPixmap(QPixmap(":/Graphics/Images/GpiDisconnected.png"));
}

void RundownFileRecorderWidget::allowGpiChanged(bool allowGpi)
{
    checkGpiTriggerable();
}

void RundownFileRecorderWidget::gpiDeviceConnected(bool connected, GpiDevice* device)
{
    checkGpiTriggerable();
}
