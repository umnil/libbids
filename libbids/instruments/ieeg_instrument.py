from typing import (
    Any,
    Callable,
    Dict,
    List,
    Tuple,
    TYPE_CHECKING,
    Union,
from .eeg_instrument import EEGInstrument
from ..enums import Modality

if TYPE_CHECKING:
    from ..session import Session  # type: ignore


class IEEGInstrument(EEGInstrument):
    """An instrumentation device capable of recording intracranial
    electroencephalograms such as electrocorticograms"""

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
        stop_fn: Union[Tuple[str, list, Dict], Callable] = lambda: None,
        is_digital: bool = False,
        **kwargs
    ):
        """Initialize a device for collecting electroecephalograms

        Parameters
        ----------
        session : Session
            Session currently using this instrument
        device : Any
            The a class that respresents a device for ieeg data collection
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
        stop_fn: Union[Tuple[str, List, Dict], Callable]
        is_digital : bool
            Whether the data recorded from the device is in a digital format or
            a physical floating point integer (e.g., µV)
        kwargs : Dict
            This keyword arguments dictionary is used to supply detailes to the
            edf file header. <See
            https://pyedflib.readthedocs.io/en/latest/_modules/pyedflib/edfwriter.html#EdfWriter.setHeader>
        """
        super(IEEGInstrument, self).__init__(
            session,
            device,
            sfreq,
            electrodes,
            physical_dimension,
            physical_lim,
            preamp_filter,
            record_duration,
            init_read_fn,
            read_fn,
            stop_fn,
            is_digital,
            **kwargs
        )
        super(EEGInstrument, self).__init__(session, Modality.iEEG, file_ext="edf")
        self.modality_path.mkdir(exist_ok=True)