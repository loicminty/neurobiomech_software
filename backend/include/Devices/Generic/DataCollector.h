#ifndef __STIMWALKER_DEVICES_GENERIC_DATA_COLLECTOR_H__
#define __STIMWALKER_DEVICES_GENERIC_DATA_COLLECTOR_H__

#include "stimwalkerConfig.h"
#include <functional>
#include <vector>

#include "Data/TimeSeries.h"
#include "Devices/Generic/Device.h"
#include "Utils/CppMacros.h"
#include "Utils/StimwalkerEvent.h"

namespace STIMWALKER_NAMESPACE::devices {

/// @brief Abstract class for data collectors
class DataCollector {
  friend class Devices;

public:
  /// @brief Constructor
  /// @param channelCount The number of channels
  /// @param timeSeriesGenerator The time series generator function
  DataCollector(size_t channelCount,
                const std::function<std::unique_ptr<data::TimeSeries>()>
                    &timeSeriesGenerator);

  /// @brief Destructor
  virtual ~DataCollector() = default;

  /// @brief Get the DataCollector name
  /// @return The name of the DataCollector
  virtual std::string dataCollectorName() const = 0;

  /// @brief Start the data streaming. This sends the data to the LiveTimeSeries
  /// but not to the TrialTimeSeries
  /// @return True if the data streaming started, false otherwise
  virtual bool startDataStreaming();

  /// @brief Stop the data streaming. This stops the data from being sent to the
  /// LiveTimeSeries and the TrialTimeSeries
  /// @return True if the data stopped streaming, false otherwise
  virtual bool stopDataStreaming();

  /// @brief Start the recording. This resets the LiveTimeSeries and starts
  /// sending the data to the TrialTimeSeries
  /// @return True if the data are recording, false otherwise
  virtual bool startRecording();

  /// @brief Stop the recording. This stops the data from being sent to the
  /// TrialTimeSeries
  /// @return True if the data are not recording, false otherwise
  virtual bool stopRecording();

protected:
  /// @brief Method to handle the start streaming data command.
  /// @return True if the data started streaming successfully, false otherwise
  virtual bool handleStartDataStreaming() = 0;

  /// @brief Method to handle the stop streaming data command.
  /// @return True if the data stopped streaming successfully, false otherwise
  virtual bool handleStopDataStreaming() = 0;

  /// @brief Get the number of channels
  /// @return The number of channels
  DECLARE_PROTECTED_MEMBER(size_t, DataChannelCount)

  /// @brief Get if the device is currently streaming data
  /// @return True if the device is streaming data, false otherwise
  DECLARE_PROTECTED_MEMBER(bool, IsStreamingData)

  /// @brief Get if the device is currently recording
  /// @return True if the device is recording, false otherwise
  DECLARE_PROTECTED_MEMBER(bool, IsRecording)

  /// @brief Has failed to start the data streaming. This is always false unless
  /// the it actually failed to start to stream
  DECLARE_PROTECTED_MEMBER(bool, HasFailedToStartDataStreaming)

  /// @brief The live time series data. This is reset when the data streaming
  /// starts
  DECLARE_PROTECTED_MEMBER_NOGET(std::unique_ptr<data::TimeSeries>,
                                 LiveTimeSeries)

  /// @brief The trial time series data. This is reset every time the recording
  /// starts
  DECLARE_PROTECTED_MEMBER_NOGET(std::unique_ptr<data::TimeSeries>,
                                 TrialTimeSeries)

public:
  const data::TimeSeries &getLiveData() const;
  const data::TimeSeries &getTrialData() const;

  /// @brief Set the callback function to call when data is collected
  /// @param callback The callback function
  StimwalkerEvent<data::DataPoint> onNewData;

protected:
  /// @brief This method adds a new data point to the data collector. This
  /// method should not be overridden (apart from AsyncDataCollector) but must
  /// be called by the inherited class when new data are ready. Once the new
  /// data are added, the [onNewData] callback is called
  /// @param data The new data to add
  virtual void addDataPoint(data::DataPoint &data);

  /// @brief Add a vector of data points to the data collector. This method is
  /// strictly equivalent to calling [addDataPoint] for each data point in the
  /// vector, except that the notification is done only once at the end. It is
  /// called with the last data point in the vector
  /// @param dataPoints The data points to add
  virtual void addDataPoints(std::vector<data::DataPoint> &dataPoints);

  /// @brief This method is useless and only serves as a reminder that the
  /// inherited class should call [addDataPoint] when new data are ready
  virtual void handleNewData(const data::DataPoint &data) = 0;
};

} // namespace STIMWALKER_NAMESPACE::devices

#endif // __STIMWALKER_DEVICES_GENERIC_DATA_COLLECTOR_H__
