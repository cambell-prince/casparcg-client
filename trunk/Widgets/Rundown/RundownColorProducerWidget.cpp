#include "RundownColorProducerWidget.h"

#include "Global.h"

#include "DeviceManager.h"
#include "GpiManager.h"
#include "Events/ConnectionStateChangedEvent.h"
#include "Events/RundownItemChangedEvent.h"

#include <QtCore/QObject>

#include <QtGui/QPixmap>

RundownColorProducerWidget::RundownColorProducerWidget(const LibraryModel& model, QWidget* parent, const QString& color, bool active,
                                                       bool loaded, bool paused, bool playing, bool inGroup, bool disconnected, bool compactView)
    : QWidget(parent),
      active(active), loaded(loaded), paused(paused), playing(playing), inGroup(inGroup), disconnected(disconnected),
      compactView(compactView), color(color), model(model)
{
    setupUi(this);

    this->animation = new ActiveAnimation(this->labelActiveColor);

    setColor(color);
    setActive(active);
    setCompactView(compactView);

    this->command.setMediaName(this->model.getName());

    this->labelDisconnected->setVisible(this->disconnected);
    this->labelGroupColor->setVisible(inGroup);
    this->labelGroupColor->setStyleSheet(QString("background-color: %1;").arg(Color::DEFAULT_GROUP_COLOR));
    this->labelColor->setStyleSheet(QString("background-color: %1;").arg(Color::DEFAULT_COLOR_PRODUCER_COLOR));

    this->labelLabel->setText(this->model.getLabel());
    this->labelChannel->setText(QString("Channel: %1").arg(this->command.getChannel()));
    this->labelVideolayer->setText(QString("Video layer: %1").arg(this->command.getVideolayer()));
    this->labelDelay->setText(QString("Delay: %1").arg(this->command.getDelay()));
    this->labelDevice->setText(QString("Device: %1").arg(this->model.getDeviceName()));

    this->executeTimer.setSingleShot(true);
    QObject::connect(&this->executeTimer, SIGNAL(timeout()), SLOT(executePlay()));

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

bool RundownColorProducerWidget::eventFilter(QObject* target, QEvent* event)
{
    if (event->type() == static_cast<QEvent::Type>(Enum::EventType::ConnectionStateChanged))
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
    else if (event->type() == static_cast<QEvent::Type>(Enum::EventType::RundownItemChanged))
    {
        // This event is not for us.
        if (!this->active)
            return false;

        RundownItemChangedEvent* rundownItemChangedEvent = dynamic_cast<RundownItemChangedEvent*>(event);
        this->model.setLabel(rundownItemChangedEvent->getLabel());
        this->model.setDeviceName(rundownItemChangedEvent->getDeviceName());
        this->model.setName(rundownItemChangedEvent->getName());
        this->command.setMediaName(rundownItemChangedEvent->getName());

        this->labelLabel->setText(this->model.getLabel());
        this->labelDevice->setText(QString("Device: %1").arg(this->model.getDeviceName()));

        checkEmptyDevice();
    }

    return QObject::eventFilter(target, event);
}

AbstractRundownWidget* RundownColorProducerWidget::clone()
{
    RundownColorProducerWidget* widget = new RundownColorProducerWidget(this->model, this->parentWidget(), this->color, this->active,
                                                                        this->loaded, this->paused, this->playing, this->inGroup,
                                                                        this->disconnected, this->compactView);

    ColorProducerCommand* command = dynamic_cast<ColorProducerCommand*>(widget->getCommand());
    command->setChannel(this->command.getChannel());
    command->setVideolayer(this->command.getVideolayer());
    command->setDelay(this->command.getDelay());
    command->setAllowGpi(this->command.getAllowGpi());
    command->setMediaName(this->command.getMediaName());
    command->setTransition(this->command.getTransition());
    command->setDuration(this->command.getDuration());
    command->setTween(this->command.getTween());
    command->setDirection(this->command.getDirection());
    command->setLoop(this->command.getLoop());
    command->setSeek(this->command.getSeek());
    command->setLength(this->command.getLength());
    command->setUseAuto(this->command.getUseAuto());

    return widget;
}

void RundownColorProducerWidget::setCompactView(bool compactView)
{
    if (compactView)
    {
        this->labelThumbnail->setFixedSize(Define::COMPACT_VIEW_WIDTH, Define::COMPACT_VIEW_HEIGHT);
        this->labelGpiConnected->setFixedSize(Define::COMPACT_VIEW_WIDTH, Define::COMPACT_VIEW_HEIGHT);
        this->labelDisconnected->setFixedSize(Define::COMPACT_VIEW_WIDTH, Define::COMPACT_VIEW_HEIGHT);
    }
    else
    {
        this->labelThumbnail->setFixedSize(Define::DEFAULT_VIEW_WIDTH, Define::DEFAULT_VIEW_HEIGHT);
        this->labelGpiConnected->setFixedSize(Define::DEFAULT_VIEW_WIDTH, Define::DEFAULT_VIEW_HEIGHT);
        this->labelDisconnected->setFixedSize(Define::DEFAULT_VIEW_WIDTH, Define::DEFAULT_VIEW_HEIGHT);
    }

    this->compactView = compactView;
}

void RundownColorProducerWidget::readProperties(boost::property_tree::wptree& pt)
{
    if (pt.count(L"color") > 0) setColor(QString::fromStdWString(pt.get<std::wstring>(L"color")));
}

void RundownColorProducerWidget::writeProperties(QXmlStreamWriter* writer)
{
    writer->writeTextElement("color", this->color);
}

bool RundownColorProducerWidget::isGroup() const
{
    return false;
}

AbstractCommand* RundownColorProducerWidget::getCommand()
{
    return &this->command;
}

LibraryModel* RundownColorProducerWidget::getLibraryModel()
{
    return &this->model;
}

void RundownColorProducerWidget::setActive(bool active)
{
    this->active = active;

    this->animation->stop();

    if (this->active)
        this->labelActiveColor->setStyleSheet("background-color: red;");
    else
        this->labelActiveColor->setStyleSheet("");
}

void RundownColorProducerWidget::setInGroup(bool inGroup)
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

void RundownColorProducerWidget::setColor(const QString& color)
{
    this->color = color;
    this->setStyleSheet(QString("#frameItem, #frameStatus { background-color: rgba(%1); }").arg(color));
}

void RundownColorProducerWidget::checkEmptyDevice()
{
    if (this->labelDevice->text() == "Device: ")
        this->labelDevice->setStyleSheet("color: black;");
    else
        this->labelDevice->setStyleSheet("");
}

bool RundownColorProducerWidget::executeCommand(enum Playout::PlayoutType::Type type)
{
    if (type == Playout::PlayoutType::Stop)
        QTimer::singleShot(0, this, SLOT(executeStop()));
    else if ((type == Playout::PlayoutType::Play && !this->command.getTriggerOnNext()) ||
             (type == Playout::PlayoutType::Update && this->model.getType() == "AUDIO"))
    {
        this->executeTimer.setInterval(this->command.getDelay());
        this->executeTimer.start();
    }
    else if (type == Playout::PlayoutType::Next && this->command.getTriggerOnNext())
        QTimer::singleShot(0, this, SLOT(executePlay()));
    else if (type == Playout::PlayoutType::Pause)
        QTimer::singleShot(0, this, SLOT(executePause()));
    else if (type == Playout::PlayoutType::Load)
        QTimer::singleShot(0, this, SLOT(executeLoad()));
    else if (type == Playout::PlayoutType::Clear)
        QTimer::singleShot(0, this, SLOT(executeClearVideolayer()));
    else if (type == Playout::PlayoutType::ClearVideolayer)
        QTimer::singleShot(0, this, SLOT(executeClearVideolayer()));
    else if (type == Playout::PlayoutType::ClearChannel)
        QTimer::singleShot(0, this, SLOT(executeClearChannel()));

    if (this->active)
        this->animation->start(1);

    return true;
}

void RundownColorProducerWidget::executeStop()
{
    this->executeTimer.stop();

    const QSharedPointer<CasparDevice> device = DeviceManager::getInstance().getConnectionByName(this->model.getDeviceName());
    if (device != NULL && device->isConnected())
        device->stopMedia(this->command.getChannel(), this->command.getVideolayer());

    foreach (const DeviceModel& model, DeviceManager::getInstance().getDeviceModels())
    {
        if (model.getShadow() == "No")
            continue;

        const QSharedPointer<CasparDevice> deviceShadow = DeviceManager::getInstance().getConnectionByName(model.getName());
        if (deviceShadow != NULL && deviceShadow->isConnected())
            deviceShadow->stopMedia(this->command.getChannel(), this->command.getVideolayer());
    }

    this->paused = false;
    this->loaded = false;
    this->playing = false;
}

void RundownColorProducerWidget::executePlay()
{
    const QSharedPointer<CasparDevice> device = DeviceManager::getInstance().getConnectionByName(this->model.getDeviceName());
    if (device != NULL && device->isConnected())
    {
        if (this->loaded)
        {
            device->playMedia(this->command.getChannel(), this->command.getVideolayer());
        }
        else
        {
            device->playMedia(this->command.getChannel(), this->command.getVideolayer(), this->command.getMediaName(),
                              this->command.getTransition(), this->command.getDuration(), this->command.getTween(),
                              this->command.getDirection(), this->command.getSeek(), this->command.getLength(),
                              this->command.getLoop(), this->command.getUseAuto());
        }
    }

    foreach (const DeviceModel& model, DeviceManager::getInstance().getDeviceModels())
    {
        if (model.getShadow() == "No")
            continue;

        const QSharedPointer<CasparDevice>  deviceShadow = DeviceManager::getInstance().getConnectionByName(model.getName());
        if (deviceShadow != NULL && deviceShadow->isConnected())
        {
            if (this->loaded)
            {
                deviceShadow->playMedia(this->command.getChannel(), this->command.getVideolayer());
            }
            else
            {
                deviceShadow->playMedia(this->command.getChannel(), this->command.getVideolayer(), this->command.getMediaName(),
                                        this->command.getTransition(), this->command.getDuration(), this->command.getTween(),
                                        this->command.getDirection(), this->command.getSeek(), this->command.getLength(),
                                        this->command.getLoop(), this->command.getUseAuto());
            }
        }
    }

    this->paused = false;
    this->loaded = false;
    this->playing = true;
}

void RundownColorProducerWidget::executePause()
{
    if (!this->playing)
        return;

    const QSharedPointer<CasparDevice> device = DeviceManager::getInstance().getConnectionByName(this->model.getDeviceName());
    if (device != NULL && device->isConnected())
    {
        if (this->paused)
            device->playMedia(this->command.getChannel(), this->command.getVideolayer());
        else
            device->pauseMedia(this->command.getChannel(), this->command.getVideolayer());
    }

    foreach (const DeviceModel& model, DeviceManager::getInstance().getDeviceModels())
    {
        if (model.getShadow() == "No")
            continue;

        const QSharedPointer<CasparDevice> deviceShadow = DeviceManager::getInstance().getConnectionByName(model.getName());
        if (deviceShadow != NULL && deviceShadow->isConnected())
        {
            if (this->paused)
                deviceShadow->playMedia(this->command.getChannel(), this->command.getVideolayer());
            else
                deviceShadow->pauseMedia(this->command.getChannel(), this->command.getVideolayer());
        }
    }

    this->paused = !this->paused;
}

void RundownColorProducerWidget::executeLoad()
{
    const QSharedPointer<CasparDevice> device = DeviceManager::getInstance().getConnectionByName(this->model.getDeviceName());
    if (device != NULL && device->isConnected())
    {
        device->loadMedia(this->command.getChannel(), this->command.getVideolayer(), this->command.getMediaName(),
                          this->command.getTransition(), this->command.getDuration(), this->command.getTween(),
                          this->command.getDirection(), this->command.getSeek(), this->command.getLength(),
                          this->command.getLoop(), this->command.getFreezeOnLoad(), this->command.getUseAuto());
    }

    foreach (const DeviceModel& model, DeviceManager::getInstance().getDeviceModels())
    {
        if (model.getShadow() == "No")
            continue;

        const QSharedPointer<CasparDevice>  deviceShadow = DeviceManager::getInstance().getConnectionByName(model.getName());
        if (deviceShadow != NULL && deviceShadow->isConnected())
        {
            deviceShadow->loadMedia(this->command.getChannel(), this->command.getVideolayer(), this->command.getMediaName(),
                                    this->command.getTransition(), this->command.getDuration(), this->command.getTween(),
                                    this->command.getDirection(), this->command.getSeek(), this->command.getLength(),
                                    this->command.getLoop(), this->command.getFreezeOnLoad(), this->command.getUseAuto());
        }
    }

    this->loaded = true;
    this->paused = false;
    this->playing = false;
}

void RundownColorProducerWidget::executeClearVideolayer()
{
    this->executeTimer.stop();

    const QSharedPointer<CasparDevice> device = DeviceManager::getInstance().getConnectionByName(this->model.getDeviceName());
    if (device != NULL && device->isConnected())
        device->clearVideolayer(this->command.getChannel(), this->command.getVideolayer());

    foreach (const DeviceModel& model, DeviceManager::getInstance().getDeviceModels())
    {
        if (model.getShadow() == "No")
            continue;

        const QSharedPointer<CasparDevice> deviceShadow = DeviceManager::getInstance().getConnectionByName(model.getName());
        if (deviceShadow != NULL && deviceShadow->isConnected())
            deviceShadow->clearVideolayer(this->command.getChannel(), this->command.getVideolayer());
    }

    this->paused = false;
    this->loaded = false;
    this->playing = false;
}

void RundownColorProducerWidget::executeClearChannel()
{
    this->executeTimer.stop();

    const QSharedPointer<CasparDevice> device = DeviceManager::getInstance().getConnectionByName(this->model.getDeviceName());
    if (device != NULL && device->isConnected())
    {
        device->clearChannel(this->command.getChannel());
        device->clearMixerChannel(this->command.getChannel());
    }

    foreach (const DeviceModel& model, DeviceManager::getInstance().getDeviceModels())
    {
        if (model.getShadow() == "No")
            continue;

        const QSharedPointer<CasparDevice> deviceShadow = DeviceManager::getInstance().getConnectionByName(model.getName());
        if (deviceShadow != NULL && deviceShadow->isConnected())
        {
            deviceShadow->clearChannel(this->command.getChannel());
            deviceShadow->clearMixerChannel(this->command.getChannel());
        }
    }

    this->paused = false;
    this->loaded = false;
    this->playing = false;
}

void RundownColorProducerWidget::channelChanged(int channel)
{
    this->labelChannel->setText(QString("Channel: %1").arg(channel));
}

void RundownColorProducerWidget::videolayerChanged(int videolayer)
{
    this->labelVideolayer->setText(QString("Video layer: %1").arg(videolayer));
}

void RundownColorProducerWidget::delayChanged(int delay)
{
    this->labelDelay->setText(QString("Delay: %1").arg(delay));
}

void RundownColorProducerWidget::checkGpiTriggerable()
{
    labelGpiConnected->setVisible(this->command.getAllowGpi());

    if (GpiManager::getInstance().getGpiDevice()->isConnected())
        this->labelGpiConnected->setPixmap(QPixmap(":/Graphics/Images/GpiConnected.png"));
    else
        this->labelGpiConnected->setPixmap(QPixmap(":/Graphics/Images/GpiDisconnected.png"));
}

void RundownColorProducerWidget::allowGpiChanged(bool allowGpi)
{
    checkGpiTriggerable();
}

void RundownColorProducerWidget::gpiDeviceConnected(bool connected, GpiDevice* device)
{
    checkGpiTriggerable();
}
