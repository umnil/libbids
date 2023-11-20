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
        sfreq: int,
        electrodes: List[str],
        physical_dimension: str = "uV",
        physical_lim: Tuple = (-1000.0, 1000.0),
        preamp_filter: str = "",
        record_duration: float = 1.0,
        init_read_fn: Union[Tuple[str, list, Dict], Callable] = lambda: None,
        read_fn: Union[Tuple[str, list, Dict], Callable] = lambda: None,
        **kwargs
    ):
        """Initialize a device for collecting electroecephalograms

        Parameters
        ----------
        session : Session
            Session currently using this instrument
        device : Any
            The a class that respresents a device for eeg data collection
        sfreq : int
            The device's sampling frequency
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
        kwargs : Dict
            This keyword arguments dictionary is used to supply detailes to the
            edf file header. <See
            https://pyedflib.readthedocs.io/en/latest/_modules/pyedflib/edfwriter.html#EdfWriter.setHeader>
        """
        super(EEGInstrument, self).__init__(session, Modality.EEG, file_ext="edf")
        self.sfreq: int = sfreq
        assert len(electrodes) > 1, "Must supply electrodes"
        self.device: Any = device
        self.electrodes: List[str] = electrodes
        self.physical_dimension: str = physical_dimension
        self.physical_lim: Tuple = physical_lim
        self.preamp_filter: str = preamp_filter
        self.record_duration: float = record_duration
        self.init_read_fn: Union[Tuple[str, List, Dict], Callable] = init_read_fn
        self.read_fn: Union[Tuple[str, List, Dict], Callable] = read_fn
        self.modality_path.mkdir(exist_ok=True)
        self.metadata: Dict = self._fixup_edf_metadata(kwargs)
        self.buffer: np.ndarray

    def annotate(self, onset: float, duration: float, description: str):
        assert self.writer.writeAnnotation(onset, duration, description) == 0

    def device_init_read(self):
        """Initializes reading on the device"""
        if isinstance(self.init_read_fn, Callable):
            self.init_read_fn()

        fn, args, kwargs = cast(Tuple, self.init_read_fn)
        self.device.__getattribute__(fn)(*args, **kwargs)

    def device_read(self) -> np.ndarray:
        """read data from the device"""
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
        # samples
        samples: np.ndarray = self.device_read()
        self.buffer = np.c_[self.buffer, samples]
        if (not remainder) and (self.buffer.shape[1] >= 256):
            writebuf: np.ndarray = self.buffer[:, :256]
            self.buffer = self.buffer[:, 256:]
            self.writer.writeSamples(np.ascontiguousarray(writebuf))
        elif remainder and (self.buffer.shape[1] > 0):
            writebuf = self.buffer[:, :256]
            self.writer.writeSamples(np.ascontiguousarray(writebuf))

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
            self.writer.setSamplefrequency(i, self.sfreq)
            if "AUX" not in el:
                self.writer.setPrefilter(i, self.preamp_filter)
