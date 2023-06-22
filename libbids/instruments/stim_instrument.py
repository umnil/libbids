from typing import Any, TYPE_CHECKING

from .instrument import Instrument
from ..enums import Modality

if TYPE_CHECKING:
    from ..session import Session


class StimInstrument(Instrument):
    """Ann instrument device capable of sending a stimulus"""

    def __init__(
        self, session: "Session", device: Any, label: str, primary_modality: Modality
    ):
        """Initialize the stimulus instrument

        Parameters
        ----------
        session : Session
            Current session
        device : Any
            The device associated with providing a stimulus
        label : str
            The label for the `recording` entity
        primary_modality : Modality
            The primary modality that this stim instrument is a companion to.
            This is typically some modality of neural recording
        """
        super(StimInstrument, self).__init__(
            session, Modality.STIM, label=label, primary_modality=primary_modality
        )
        self.device: Any = device

    def start(self, task: str, run_id: str):
        self.device.start()

    def stop(self):
        self.device.stop()

    def write(self, command: str):
        """a string command to send to the device"""
        self.device.write(command)
