#include "ColorCommand.h"

#include "Global.h"

ColorCommand::ColorCommand(QObject* parent)
    : QObject(parent),
      channel(Output::DEFAULT_CHANNEL), videolayer(Output::DEFAULT_VIDEOLAYER), delay(Output::DEFAULT_DELAY),
      allowGpi(Output::DEFAULT_ALLOW_GPI), color(Color::DEFAULT_COLOR_NAME), transition(Mixer::DEFAULT_TRANSITION),
      duration(Mixer::DEFAULT_DURATION), tween(Mixer::DEFAULT_TWEEN), direction(Mixer::DEFAULT_DIRECTION)
{
}

int ColorCommand::getDelay() const
{
    return this->delay;
}

int ColorCommand::getChannel() const
{
    return this->channel;
}

int ColorCommand::getVideolayer() const
{
    return this->videolayer;
}

bool ColorCommand::getAllowGpi() const
{
    return this->allowGpi;
}

void ColorCommand::setChannel(int channel)
{
    this->channel = channel;
    emit channelChanged(this->channel);
}

void ColorCommand::setVideolayer(int videolayer)
{
    this->videolayer = videolayer;
    emit videolayerChanged(this->videolayer);
}

void ColorCommand::setDelay(int delay)
{
    this->delay = delay;
    emit delayChanged(this->delay);
}

void ColorCommand::setAllowGpi(bool allowGpi)
{
    this->allowGpi = allowGpi;
    emit allowGpiChanged(this->allowGpi);
}

const QString& ColorCommand::getColor() const
{
    return this->color;
}

const QString& ColorCommand::getTransition() const
{
    return this->transition;
}

int ColorCommand::getDuration() const
{
    return this->duration;
}

const QString& ColorCommand::getDirection() const
{
    return this->direction;
}

const QString& ColorCommand::getTween() const
{
    return this->tween;
}

void ColorCommand::setColor(const QString& color)
{
    this->color = color;
    emit colorChanged(this->color);
}

void ColorCommand::setTransition(const QString& transition)
{
    this->transition = transition;
    emit transitionChanged(this->transition);
}

void ColorCommand::setDuration(int duration)
{
    this->duration = duration;
    emit durationChanged(this->duration);
}

void ColorCommand::setDirection(const QString& direction)
{
    this->direction = direction;
    emit directionChanged(this->direction);
}

void ColorCommand::setTween(const QString& tween)
{
    this->tween = tween;
    emit tweenChanged(this->tween);
}

void ColorCommand::readProperties(boost::property_tree::wptree& pt)
{
    if (pt.count(L"channel") > 0) setChannel(pt.get<int>(L"channel"));
    if (pt.count(L"videolayer") > 0) setVideolayer(pt.get<int>(L"videolayer"));
    if (pt.count(L"delay") > 0) setDelay(pt.get<int>(L"delay"));
    if (pt.count(L"allowgpi") > 0) setAllowGpi(pt.get<bool>(L"allowgpi"));
    if (pt.count(L"transition") > 0) setTransition(QString::fromStdWString(pt.get<std::wstring>(L"transition")));
    if (pt.count(L"duration") > 0) setDuration(pt.get<int>(L"duration"));
    if (pt.count(L"tween") > 0) setTween(QString::fromStdWString(pt.get<std::wstring>(L"tween")));
    if (pt.count(L"direction") > 0) setDirection(QString::fromStdWString(pt.get<std::wstring>(L"direction")));
    if (pt.count(L"color") > 0) setColor(QString::fromStdWString(pt.get<std::wstring>(L"color")));
}

void ColorCommand::writeProperties(QXmlStreamWriter* writer)
{
    writer->writeTextElement("channel", QString::number(this->getChannel()));
    writer->writeTextElement("videolayer", QString::number(this->getVideolayer()));
    writer->writeTextElement("delay", QString::number(this->getDelay()));
    writer->writeTextElement("allowgpi", (getAllowGpi() == true) ? "true" : "false");
    writer->writeTextElement("transition", this->getTransition());
    writer->writeTextElement("duration", QString::number(this->getDuration()));
    writer->writeTextElement("tween", this->getTween());
    writer->writeTextElement("direction", this->getDirection());
    writer->writeTextElement("color", this->getColor());
}
