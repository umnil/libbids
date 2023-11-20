"""Instrument for stimulating or recording"""
from abc import ABC, abstractmethod
from pathlib import Path
from typing import Optional, TYPE_CHECKING, Union, cast

from ..enums import Modality

if TYPE_CHECKING:
    from ..session import Session  # type: ignore


class Instrument(ABC):
    def __init__(
        self,
        session: "Session",
        modality: Union[Modality, str],
        label: Optional[str] = None,
        primary_modality: Optional[Union[Modality, str]] = None,
        file_ext: str = "tsv",
    ):
        """Initialize an instrument that will write BIDS data

        Parameters
        ----------
        session : Session
            The session that this instrument will be used in
        modality : Union[Modality, str]
            The type of data that this instrument is responsible for recording
            or stimulating
        label : Optional[str]
            Only used for STIM and PHYSIO modalities. Will ensure any written
            data files corresponding to this instrument have the appropriate
            `recording-<label>` tag appended to files
        primary_modality : Optional[Union[Modality, str]]
            If the istrument is a STIM or PHYSIO, this indiciates to which
            modality the data files should be written.
        file_ext : str
            The file extension of the associated file to save
        """
        self.session: "Session" = session
        self.modality: Modality = (
            cast(Modality, modality)
            if isinstance(modality, Modality)
            else cast(Modality, Modality._member_map_[modality])
        )

        if label is not None:
            assert not self.modality.is_primary
            assert primary_modality is not None
            self.label: str = label
            self.primary_modality: Modality = (
                cast(Modality, primary_modality)
                if isinstance(primary_modality, Modality)
                else cast(Modality, Modality._member_map_[cast(str, primary_modality)])
            )
            assert self.primary_modality.is_primary

        self.file_ext: str = file_ext

        self.task_id: str = ""
        self.run_id: str = ""
        self._started: bool = False
        self.sfreq: int

    @abstractmethod
    def start(self, task_id: str, run_id: str):
        self.task_id = task_id
        self.run_id = run_id
        self._started = True

    @abstractmethod
    def stop(self):
        self.task_id = ""
        self.run_id = ""
        self._started = False

    @property
    def filename(self) -> str:
        return (
            "_".join(
                [
                    self.run_prefix,
                    ("" if not hasattr(self, "label") else f"recording-{self.label}_")
                    + self.modality.name.lower(),
                ]
            )
            + f".{self.file_ext}"
        )

    @property
    def filepath(self) -> Path:
        return self.modality_path.joinpath(self.filename)

    @property
    def modality_path(self) -> Path:
        return self.session.path.joinpath(
            self.modality.name.lower()
            if self.modality.is_primary
            else self.primary_modality.name.lower()
        )

    @property
    def run_prefix(self) -> str:
        assert self.task_id != ""
        assert self.run_id != ""
        return "_".join([self.subject_id, self.session_id, self.task_id, self.run_id])

    @property
    def session_id(self) -> str:
        return self.session.id

    @property
    def subject_id(self) -> str:
        return self.session.subject.path.name
