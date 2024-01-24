import numpy as np
from abc import abstractmethod
from typing import List, Union

from .instrument import Instrument


class ReadInstrument(Instrument):
    """An instrument device capabale of recording data"""

    @abstractmethod
    def flush(self) -> None:
        """Read from the device but throw away the data as a way to
        clear any data buffers from the device"""
        raise Exception("Method not implemented")

    @abstractmethod
    def read(self, remainder: bool = False) -> Union[List, np.ndarray]:
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
        raise Exception("Method not implemented")
