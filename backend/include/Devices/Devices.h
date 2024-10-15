#ifndef __STIMWALKER_DEVICES_DATA_DEVICES_DATA_H__
#define __STIMWALKER_DEVICES_DATA_DEVICES_DATA_H__

#include "stimwalkerConfig.h"

#include "Utils/CppMacros.h"
#include <nlohmann/json.hpp>

namespace STIMWALKER_NAMESPACE {
namespace data {
class TimeSeries;
} // namespace data

namespace devices {
class DataCollector;
class Device;

/// @brief Class to store data
class Devices {

  /// DEVICE MANAGEMENT METHODS ///
public:
  /// @brief Constructor
  Devices() = default;
  ~Devices();

  /// @brief Create a new device in the collection
  /// @param device The device to add
  /// @return The id of the device in the collection so it can be accessed or
  /// removed later
  int add(std::unique_ptr<Device> device);

  /// @brief Remove the device from the collection
  /// @param deviceId The id of the device (the one returned by the add method)
  void remove(int deviceId);

  /// @brief Get the number of devices in the collection
  /// @return The number of devices in the collection
  size_t size() const;

  /// @brief Remove all the devices from the collection
  void clear();

  /// @brief Get the requested device
  /// @param deviceId The id of the device (the one returned by the add method)
  /// @return The requested device
  const Device &operator[](int deviceId) const;

  /// @brief Get the requested device
  /// @param deviceId The id of the device (the one returned by the add method)
  /// @return The requested device
  const Device &getDevice(int deviceId) const;

  /// @brief Get the requested data collector
  /// @param deviceId The id of the data collector
  /// @return The requested data collector
  const DataCollector &getDataCollector(int deviceId) const;

  /// DRIVING THE DEVICES METHODS ///
public:
  /// @brief Connect all the devices in a blocking way (wait for all the devices
  /// to be connected before returning)
  bool connect();

  /// @brief Disconnect all the devices in a blocking way (wait for all the
  /// devices to be disconnected before returning)
  bool disconnect();

  /// @brief Start recording on all the devices in a blocking way (wait for all
  /// the devices to start recording before returning). Additionally, it sets
  /// all the starting reconding time of all device to "now" as of the time of
  /// the when all the devices are started.
  /// @return True if all the devices started recording, false otherwise
  bool startRecording();

  /// @brief Stop recording on all the devices in a blocking way (wait for all
  /// the devices to stop recording before returning)
  bool stopRecording();

  /// @brief Pause recording on all the devices in a blocking way (wait for all
  /// the devices to pause recording before returning)
  void pauseRecording();

  /// @brief Resume recording on all the devices in a blocking way (wait for all
  /// the devices to resume recording before returning)
  void resumeRecording();

  /// DATA SPECIFIC METHODS ///
public:
  /// @brief Get the data in serialized form
  /// @return The data in serialized form
  nlohmann::json serialize() const;

  /// INTERNAL ///
protected:
  /// @brief If the devices are connected
  DECLARE_PROTECTED_MEMBER(bool, IsConnected)

  /// @brief If the devices are recording
  DECLARE_PROTECTED_MEMBER(bool, IsRecording)

  /// @brief If the devices are recording
  DECLARE_PROTECTED_MEMBER(bool, IsPaused)

protected:
  /// @brief The collection of devices
  std::map<int, std::shared_ptr<Device>> m_Devices;

public:
  /// @brief The collection of data collectors
  const std::map<int, std::shared_ptr<DataCollector>> &
  getDataCollectors() const {
    return m_DataCollectors;
  }
  std::map<int, std::shared_ptr<DataCollector>> m_DataCollectors;
};

} // namespace devices
} // namespace STIMWALKER_NAMESPACE

#endif // __STIMWALKER_DEVICES_DATA_DEVICES_DATA_H__