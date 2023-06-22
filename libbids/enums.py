from enum import Enum, auto


class Modality(Enum):
    MRI = auto()
    MEG = auto()
    EEG = auto()
    iEEG = auto()
    PHYSIO = auto()
    STIM = auto()

    @property
    def is_primary(self) -> bool:
        return self.name not in ["PHYSIO", "STIM"]
