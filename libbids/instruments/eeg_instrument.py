import numpy as np  # type: ignore

from datetime import datetime
from pyedflib import EdfWriter  # type: ignore
from typing import (
    Any,
    Callable,
    Dict,
    List,
    Tuple,
    TYPE_CHECKING,
    Union,
    cast,
)

from .read_instrument import ReadInstrument
from ..enums import Modality

if TYPE_CHECKING:
    from ..session import Session  # type: ignore


class EEGInstrument(ReadInstrument):
    """An instrumentation device capable of recording electroecephalograms"""

    def __init__(
        self,
        session: "Session",
        device: Any,
        sfreq: Union[int, List[int]],
        electrodes: List[str],
        physical_dimension: str = "uV",
        physical_lim: Tuple = (-1000.0, 1000.0),
        preamp_filter: str = "",
        record_duration: float = 1.0,
        init_read_fn: Union[Tuple[str, list, Dict], Callable] = lambda: None,
        read_fn: Union[Tuple[str, list, Dict], Callable] = lambda: None,
        is_digital: bool = False,
        **kwargs
    ):
        """Initialize a device for collecting electroecephalograms

        Parameters
        ----------
        session : Session
            Session currently using this instrument
        device : Any
            The a class that respresents a device for eeg data collection
        sfreq : Union[int, List[int]]
            The device's sampling frequency. All channels/electrodes assume the
            same sampling rate if a signle integer is provided, else a list of
            sampling rates must be provided for each channel/electrode
        electrodes: List[str]
            A list of electrode names associated with the device
        physical_dimension : str
            The units of the signal
        physical_lim : Tuple
            The limits of the physical dimension
        preamp_filter : str
            Optionally supplied preamp filter settings for saving into the data
            of the edf file
        record_duration: float
            A length in time in secon of each data record chunk stored to the
            edf file
        init_read_fn : Union[Tuple[str, List, Dict], Callable]
            If a tuple of a string and then a dictionary, the first string is
            the function name on `device` used for initializing reading, and the
            list is args and the Dict is the kwargs. If this argument is
            callable, the function is simply called
        read_fn : Union[Tuple[str, Dict], Callable]
            Similar to `init_read_fn`, but used for sampling data from the device
        is_digital : bool
            Whether the data recorded from the device is in a digital format or
            a physical floating point integer (e.g., µV)
        kwargs : Dict
            This keyword arguments dictionary is used to supply detailes to the
            edf file header. <See
            https://pyedflib.readthedocs.io/en/latest/_modules/pyedflib/edfwriter.html#EdfWriter.setHeader>
        """
        super(EEGInstrument, self).__init__(session, Modality.EEG, file_ext="edf")
        self.sfreqs: List[int] = sfreq if isinstance(sfreq, List) else [sfreq]
        assert len(electrodes) > 1, "Must supply electrodes"
        assert len(self.sfreqs) == 1 or len(self.sfreqs) == len(
            electrodes
        ), "Must supply same number of sampling rates as electrodes"
        self.device: Any = device
        self.electrodes: List[str] = electrodes
        self.physical_dimension: str = physical_dimension
        self.physical_lim: Tuple = physical_lim
        self.preamp_filter: str = preamp_filter
        self.record_duration: float = record_duration
        self.init_read_fn: Union[Tuple[str, List, Dict], Callable] = init_read_fn
        self.read_fn: Union[Tuple[str, List, Dict], Callable] = read_fn
        self.is_digital: bool = is_digital
        self.modality_path.mkdir(exist_ok=True)
        self.metadata: Dict = self._fixup_edf_metadata(kwargs)
        self.buffer: np.ndarray
        self.buffers: List[np.ndarray] = [np.array([]) for i in range(len(self.sfreqs))]

    def annotate(self, onset: float, duration: float, description: str):
        assert self.writer.writeAnnotation(onset, duration, description) == 0

    def device_init_read(self):
        """Initializes reading on the device"""
        if isinstance(self.init_read_fn, Callable):
            self.init_read_fn()

        fn, args, kwargs = cast(Tuple, self.init_read_fn)
        self.device.__getattribute__(fn)(*args, **kwargs)

    def device_read(self) -> Union[np.ndarray, List]:
        """read data from the device

        Returns
        -------
        np.ndarray
            If all channels share the same sampling rate
        List
            If not all channels share the same sampling rate
        """
        if isinstance(self.read_fn, Callable):  # type: ignore
            return cast(Callable, self.read_fn)()

        fn, args, kwargs = cast(Tuple, self.read_fn)
        return self.device.__getattribute__(fn)(*args, **kwargs)

    def flush(self) -> None:
        """Read data from the device simply to discard"""
        self.device_read()

    def read(self, remainder: bool = False) -> np.ndarray:
        """Read data from the headset and return the data

        Parameters
        ----------
        remainder : bool
            Because EDFWriter only allows full seconds to be written, the last
            time this function should be called is with remainder set to true

        Returns
        -------
        np.ndarray
            A 2D array of data in the shape of (channels, time)
        """
        if len(self.sfreqs) == 1:
            sfreq: int = self.sfreqs[0]
            period: int = int(sfreq * self.record_duration)
            samples: np.ndarray = self.device_read()
            self.buffer = np.c_[self.buffer, samples]
            if (not remainder) and (self.buffer.shape[1] >= period):
                writebuf: np.ndarray = self.buffer[:, :period]
                self.buffer = self.buffer[:, period:]
                self.writer.writeSamples(
                    np.ascontiguousarray(writebuf), digital=self.is_digital
                )
            elif remainder and (self.buffer.shape[1] > 0):
                writebuf = self.buffer[:, :period]
                self.writer.writeSamples(
                    np.ascontiguousarray(writebuf), digital=self.is_digital
                )
        else:
            periods: List = [int(f) * self.record_duration for f in self.sfreqs]
            ch_samples: List = self.device_read()
            assert len(ch_samples) == len(
                self.sfreqs
            ), "Data must be the same length as the number sfreqs"
            self.buffers = [
                np.c_[i, j]
                for i, j in zip(
                    self.buffers,
                )
            ]
            period_met: bool = np.all(
                [i.shape[0] >= j for i, j in zip(self.buffers, periods)]
            )
            has_data: bool = np.any([i.shape[0] > 0 for i in self.buffers])
            if (not remainder) and period_met:
                writebufs: List = [i[:j] for i, j in zip(self.buffers, periods)]
                self.buffers = [i[j:] for i, j in zip(self.buffers, periods)]
                self.writer.writeSamples(writebufs, digital=self.is_digital)
            elif remainder and has_data:
                writebufs = [i[:j] for i, j in zip(self.buffers, periods)]
                self.writer.writeSamples(writebufs, digital=self.is_digital)

        return samples

    def start(self, task: str, run_id: str):
        """Begin recording a run

        Parameters
        ----------
        task : str
            The name of the task to be applied to the recorded file data
        run_id : str
            The id of the run that will be appended to the file
        """
        super().start(task, run_id)
        n_electrodes: int = len(self.electrodes)
        self._initialize_edf_file()
        self.buffer = np.empty(shape=(n_electrodes, 0))
        self.writer.setStartdatetime(datetime.now())
        self.device_init_read()

    def stop(self):
        """Stop the run"""
        super().stop()
        self.writer.close()

    def _fixup_edf_metadata(self, metadata: Dict):
        """A dictionary of values that will be used to store edf metadata

        Parameters
        ----------
        metadata : Dict
            The dictionary that will be parsed
        """
        required_keys: List[str] = [
            "technician",
            "recording_additional",
            "patientname",
            "patient_additional",
            "patientcode",
            "equipment",
            "admincode",
            "gender",
            "sex",
            "startdate",
            "birthdate",
        ]
        result: Dict = {}
        for required_key in required_keys:
            if required_key in metadata.keys():
                value = metadata[required_key]
            else:
                value = ""
                if required_key == "startdate":
                    value = datetime.now()
            result.update({required_key: value})
        return result

    def _initialize_edf_file(self) -> None:
        """Initialize the EDF file that will save the data collected from this
        instrument"""
        edf_fp: str = str(self.filepath)
        n_electrodes: int = len(self.electrodes)
        self.writer: EdfWriter = EdfWriter(edf_fp, n_electrodes)
        self.writer.setHeader(self.metadata)
        self.writer.setDatarecordDuration(self.record_duration)
        for i, el in enumerate(self.electrodes):
            self.writer.setLabel(i, el)
            self.writer.setPhysicalDimension(i, self.physical_dimension)
            self.writer.setPhysicalMaximum(i, self.physical_lim[0])
            self.writer.setPhysicalMinimum(i, self.physical_lim[1])
            self.writer.setSamplefrequency(
                i, self.sfreqs[0] if len(self.sfreqs) == 1 else self.sfreqs[i]
            )
            if "AUX" not in el:
                self.writer.setPrefilter(i, self.preamp_filter)
