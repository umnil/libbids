from abc import abstractmethod

from .instrument import Instrument


class WriteInstrument(Instrument):

    @abstractmethod
    def write(self, command: str) -> None:
        """A string command to send to the device

        Parameters
        ----------
        command : str
            The command to send to the device
        """
        raise Exception("Method not implemented")
