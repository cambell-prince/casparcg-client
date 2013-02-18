#include "GpiDevice.h"

#include <boost/bind.hpp>

namespace gpio
{

class serial_port_device::impl {};

serial_port_device::serial_port_device(
        const std::string& serial_port,
        int baud_rate,
        const connection_listener& connection_listener) {}

serial_port_device::ptr
serial_port_device::create(
        const std::string& serial_port,
        int baud_rate,
        const connection_listener& connection_listener) { return ptr(new serial_port_device(serial_port,baud_rate, connection_listener)); }

serial_port_device::~serial_port_device() {}

int serial_port_device::get_num_gpi_ports() const { return 0; }

int serial_port_device::get_num_gpo_ports() const { return 0; }

void serial_port_device::setup_gpi_pulse(
        int gpi_port,
        voltage silent_state,
        const gpi_trigger_handler& handler) {}

void serial_port_device::setup_gpi_tally(
        int gpi_port,
        voltage off_state,
        const gpi_switch_handler& handler) {}

void serial_port_device::stop_gpi(int gpi_port) {}

gpo_trigger::ptr serial_port_device::setup_gpo_pulse(
        int gpo_port, voltage silent_state, int duration_milliseconds) { return gpo_trigger::ptr(); }

gpo_switch::ptr serial_port_device::setup_gpo_tally(int gpo_port, voltage off_state) { return gpo_switch::ptr(); }

}

GpiDevice::GpiDevice(const QString& serialPort, int baudRate)
    : device(gpio::serial_port_device::create(serialPort.toStdString(), baudRate, boost::bind(&GpiDevice::onConnected, this, _1)))
{
}

void GpiDevice::reset(const QString& serialPort, int baudRate)
{
    emit connectionStateChanged(false, this);
    connected = false;
    gpo_triggers.clear();
    device.reset();
    device = gpio::serial_port_device::create(serialPort.toStdString(), baudRate, boost::bind(&GpiDevice::onConnected, this, _1));
}

void GpiDevice::setupGpiPort(int gpi, bool risingEdge)
{
    device->setup_gpi_pulse(gpi, risingEdge ? gpio::LOW : gpio::HIGH, boost::bind(&GpiDevice::onTrigger, this, gpi));
}

void GpiDevice::setupGpoPort(int gpo, int pulseLengthMillis, bool risingEdge)
{
    gpo_triggers.erase(gpo);
    gpo_triggers.insert(std::make_pair(gpo, device->setup_gpo_pulse(gpo, risingEdge ? gpio::LOW : gpio::HIGH, pulseLengthMillis)));
}

void GpiDevice::onTrigger(int gpi)
{
    emit gpiTriggered(gpi, this);
}

void GpiDevice::onConnected(bool state)
{
    connected = state;
    emit connectionStateChanged(state, this);
}

bool GpiDevice::isConnected()
{
    return connected;
}

void GpiDevice::trigger(int gpo)
{
    std::map<int, gpio::gpo_trigger::ptr>::iterator iter = gpo_triggers.find(gpo);

    if (iter != gpo_triggers.end())
        iter->second->fire();
    else
        throw std::runtime_error("GPO not setup for port");
}

int GpiDevice::getNumGpi()
{
    return device->get_num_gpi_ports();
}

int GpiDevice::getNumGpo()
{
    return device->get_num_gpo_ports();
}
