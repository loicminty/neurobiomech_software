from abc import ABC, abstractmethod
from typing import Callable

import numpy as np

from .data import NiDaqData


class NiDaqGeneric(ABC):
    def __init__(
        self, num_channels: int, frame_rate: int, on_data_ready: Callable[[np.ndarray, np.ndarray], None] | None = None
    ) -> None:
        """
        Parameters
        ----------
        num_channels : int
            Number of channels connected to the NiDaq
        rate : int
            Frames per second
        on_data_ready : Callable[[np.ndarray, np.ndarray], None] | None
            Callback function that is called when new data is ready (the new time and data are passed as arguments)
        """
        self._num_channels = num_channels
        self._frame_rate = frame_rate
        self._time_between_samples: float = 1  # second

        # Data releated variables
        self._data: NiDaqData = NiDaqData()

        # Callback function that is called when new data are added
        self._is_recording: bool = False
        self._on_data_ready_callback = on_data_ready

        # Setup the NiDaq task
        self._task = None
        self._setup_task()

    @property
    def num_channels(self) -> int:
        """Number of channels connected to the NiDaq"""
        return self._num_channels

    @property
    def frame_rate(self) -> int:
        """Frames per second"""
        return self._frame_rate

    @property
    def dt(self) -> float:
        """Time between samples"""
        return 1 / self.frame_rate

    def start_recording(self):
        """Start recording"""
        if self._is_recording:
            raise RuntimeError("Already recording")

        self._reset_data()
        self._start_task()
        self._is_recording = True

    def stop_recording(self):
        """Stop recording"""
        if not self._is_recording:
            # If we are not currently recording, we don't need to stop the recording
            return

        self._stop_task()
        self._is_recording = False

    @property
    def data(self) -> NiDaqData:
        """Data from the NiDaq"""
        return self._data.copy

    def dispose(self):
        """Dispose the NiDaq class"""
        self.stop_recording()
        if self._task is not None:
            self._task.close()
            self._task = None

    def _reset_data(self):
        """Reset data to start a new trial"""
        self._data = NiDaqData()

    def _data_has_arrived(self, data: np.ndarray):
        """
        Callback function for reading signals.
        It automatically computes the time vector and calls the callback function if it exists.
        """

        prev_t, _ = self._data.sample_block(index=-1, unsafe=True)
        t0 = 0 if prev_t is None else (prev_t[-1] + self.dt)
        t = np.linspace(t0, t0 + self._time_between_samples - self.dt, self.frame_rate)

        self._data.add(t, data)

        if self._on_data_ready_callback is not None:
            self._on_data_ready_callback(*self._data.sample_block(index=-1, unsafe=True))

        return 1

    def _setup_task(self):
        """Setup the NiDaq task"""
        import nidaqmx
        from nidaqmx.constants import AcquisitionType

        self._task = nidaqmx.Task()  # This replaces the usual with statement
        for i in range(self.num_channels):
            self._task.ai_channels.add_ai_voltage_chan(self._channel_name(i))

        self._task.timing.cfg_samp_clk_timing(self.frame_rate, sample_mode=AcquisitionType.CONTINUOUS)

        n_samples = self._time_between_samples * self.frame_rate
        self._task.register_every_n_samples_acquired_into_buffer_event(
            n_samples,
            lambda *_, **__: self._data_has_arrived(np.array(self._task.read(number_of_samples_per_channel=n_samples))),
        )

    @abstractmethod
    def _channel_name(self, channel: int) -> str:
        """Channel name assuming it is the channel number [channel]"""

    def _start_task(self):
        """Start the NiDaq task"""
        self._task.start()

    def _stop_task(self):
        self._task.stop()
