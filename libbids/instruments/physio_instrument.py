import numpy as np
from datetime import datetime
from typing import Any, Callable, Dict, List, Tuple, Union, TYPE_CHECKING, cast
from .instrument import Instrument
from ..enums import Modality

if TYPE_CHECKING:
    from ..session import Session


class PhysioInstrument(Instrument):
    """An instrument device capable of sampling data from a device"""

    def __init__(
        self,
        session: "Session",
        device: Any,
        sfreq: int,
        electrodes: List[str],
        physical_dimension: str = "mV",
        physical_lim: Tuple = (-500.0, 500.0),
        preamp_filter: str = "",
        init_read_fn: Union[Tuple[str, list, Dict], Callable] = lambda: None,
        read_fn: Union[Tuple[str, list, Dict], Callable] = lambda: None,
        **kwargs
    ):
        """Initialize a device for collecting physiology data

        Parameters
        ----------
        session : Session
            Session currently using this instrument
        device : Any
            The a class that respresents a device hardware for recording
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
        super(PhysioInstrument, self).__init__(
            session,
            Modality.PHYSIO,
            label="emg",
            primary_modality=Modality.EEG,
            file_ext="edf",
        )
        self.sfreq: int = sfreq
        assert len(electrodes) > 1, "Must supply electrodes"
        self.device: Any = device
        self.electrodes: List[str] = electrodes
        self.physical_dimension: str = physical_dimension
        self.physical_lim: Tuple = physical_lim
        self.preamp_filter: str = preamp_filter
        self.init_read_fn: Union[Tuple[str, List, Dict], Callable] = init_read_fn
        self.read_fn: Union[Tuple[str, List, Dict], Callable] = read_fn
        self.modality_path.mkdir(exist_ok=True)
        self.metadata: Dict = self._fixup_edf_metadata(kwargs)
        self.buffer: np.ndarray

    def device_init_read(self, *args):
        """Initializes reading on the device"""
        if isinstance(self.init_read_fn, Callable):
            self.init_read_fn()

        fn, pargs, kwargs = cast(Tuple, self.init_read_fn)
        args += tuple(pargs)
        self.device.__getattribute__(fn)(*args, **kwargs)

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
        self.device_init_read(task, run_id)

    def stop(self):
        self.device.stop(
            metadata=self.metadata,
            edf_filepath=self.filepath,
            electrodes=self.electrodes,
        )
        super(PhysioInstrument, self).stop()

    def write(self, event_data: str):
        """TTL pulse

        Parameters
        ----------
        event_data: str
        """
        self.device.write(event_data)

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

