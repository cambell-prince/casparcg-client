#include "MediaCommand.h"

#include "Global.h"

MediaCommand::MediaCommand(QObject* parent)
    : AbstractCommand(parent),
      mediaName(Media::DEFAULT_MEDIA_NAME), transition(Mixer::DEFAULT_TRANSITION), duration(Mixer::DEFAULT_DURATION),
      tween(Mixer::DEFAULT_TWEEN), direction(Mixer::DEFAULT_DIRECTION), loop(Media::DEFAULT_LOOP),
      freezeOnLoad(Media::DEFAULT_FREEZE_ON_LOAD), triggerOnNext(Media::DEFAULT_TRIGGER_ON_NEXT), seek(Media::DEFAULT_SEEK),
      length(Media::DEFAULT_LENGTH), useAuto(Media::DEFAULT_USE_AUTO)
{
}

const QString& MediaCommand::getMediaName() const
{
    return this->mediaName;
}

const QString& MediaCommand::getTransition() const
{
    return this->transition;
}

int MediaCommand::getDuration() const
{
    return this->duration;
}

const QString& MediaCommand::getDirection() const
{
    return this->direction;
}

const QString& MediaCommand::getTween() const
{
    return this->tween;
}

bool MediaCommand::getLoop() const
{
    return this->loop;
}

bool MediaCommand::getFreezeOnLoad() const
{
    return this->freezeOnLoad;
}

bool MediaCommand::getTriggerOnNext() const
{
    return this->triggerOnNext;
}

int MediaCommand::getSeek() const
{
    return this->seek;
}

int MediaCommand::getLength() const
{
    return this->length;
}

bool MediaCommand::getUseAuto() const
{
    return this->useAuto;
}

void MediaCommand::setMediaName(const QString& mediaName)
{
    this->mediaName = mediaName;
    emit mediaNameChanged(this->mediaName);
}

void MediaCommand::setTransition(const QString& transition)
{
    this->transition = transition;
    emit transitionChanged(this->transition);
}

void MediaCommand::setDuration(int duration)
{
    this->duration = duration;
    emit durationChanged(this->duration);
}

void MediaCommand::setDirection(const QString& direction)
{
    this->direction = direction;
    emit directionChanged(this->direction);
}

void MediaCommand::setTween(const QString& tween)
{
    this->tween = tween;
    emit tweenChanged(this->tween);
}

void MediaCommand::setLoop(bool loop)
{
    this->loop = loop;
    emit loopChanged(this->loop);
}

void MediaCommand::setFreezeOnLoad(bool freezeOnLoad)
{
    this->freezeOnLoad = freezeOnLoad;
    emit freezeOnLoadChanged(this->freezeOnLoad);
}

void MediaCommand::setTriggerOnNext(bool triggerOnNext)
{
    this->triggerOnNext = triggerOnNext;
    emit triggerOnNextChanged(this->triggerOnNext);
}

void MediaCommand::setSeek(int seek)
{
    this->seek = seek;
    emit seekChanged(this->seek);
}

void MediaCommand::setLength(int length)
{
    this->length = length;
    emit lengthChanged(this->length);
}

void MediaCommand::setUseAuto(bool useAuto)
{
    this->useAuto = useAuto;
    emit useAutoChanged(this->useAuto);
}

void MediaCommand::readProperties(boost::property_tree::wptree& pt)
{
    AbstractCommand::readProperties(pt);

    if (pt.count(L"transition") > 0) setTransition(QString::fromStdWString(pt.get<std::wstring>(L"transition")));
    if (pt.count(L"duration") > 0) setDuration(pt.get<int>(L"duration"));
    if (pt.count(L"tween") > 0) setTween(QString::fromStdWString(pt.get<std::wstring>(L"tween")));
    if (pt.count(L"direction") > 0) setDirection(QString::fromStdWString(pt.get<std::wstring>(L"direction")));
    if (pt.count(L"seek") > 0) setSeek(pt.get<int>(L"seek"));
    if (pt.count(L"length") > 0) setLength(pt.get<int>(L"length"));
    if (pt.count(L"loop") > 0) setLoop(pt.get<bool>(L"loop"));
    if (pt.count(L"freezeonload") > 0) setFreezeOnLoad(pt.get<bool>(L"freezeonload"));
    if (pt.count(L"useauto") > 0) setUseAuto(pt.get<bool>(L"useauto"));
    if (pt.count(L"triggeronnext") > 0) setTriggerOnNext(pt.get<bool>(L"triggeronnext"));
}

void MediaCommand::writeProperties(QXmlStreamWriter* writer)
{
    AbstractCommand::writeProperties(writer);

    writer->writeTextElement("transition", this->getTransition());
    writer->writeTextElement("duration", QString::number(this->getDuration()));
    writer->writeTextElement("tween", this->getTween());
    writer->writeTextElement("direction", this->getDirection());
    writer->writeTextElement("seek", QString::number(this->getSeek()));
    writer->writeTextElement("length", QString::number(this->getLength()));
    writer->writeTextElement("loop", (getLoop() == true) ? "true" : "false");
    writer->writeTextElement("freezeonload", (getFreezeOnLoad() == true) ? "true" : "false");
    writer->writeTextElement("useauto", (getUseAuto() == true) ? "true" : "false");
    writer->writeTextElement("triggeronnext", (getTriggerOnNext() == true) ? "true" : "false");
}
