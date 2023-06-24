"""
Defines an object that represents a subject
"""
import os
import json
import pandas as pd  # type: ignore
from pathlib import Path
from PyQt6.QtWidgets import QMessageBox  # type: ignore
from typing import Any, Dict, List, TYPE_CHECKING, Union
from clibbids import Entity, Session

if TYPE_CHECKING:
    from .dataset import Dataset


class Subject(Entity):
    def __init__(self, dataset: "Dataset", **kwargs):
        """Initialize a new subject instance

        Parameters
        ----------
        dataset : Dataset
            The dataset to which this subject belongs
        kwargs : Dict
            A dictionary of information provided by the dataset when creating.
            The keyward arguments (exlcuding `participant_id` are defined by the
            keys in the participants.json sidecare). Participant_id must be a
            defined keyword argument in the full entity specification; e.g., `sub-001`
        """
        self.dataset: "Dataset" = dataset
        for k, v in kwargs.items():
            self.__setattr__(k, v)
        self.participant_id: str = self.ensure_participant_id(self.participant_id)
        self.participant_name: str = self.name
        self.path.mkdir(exist_ok=True)
        super(Subject, self).__init__("Subject", "sub", self.participant_label)

    def __getitem__(self, key: str) -> Any:
        return self.__getattribute__(key)

    def add_session(self, force_new: bool = False) -> Session:
        """Add a session to this subject. Prompt for confirmation that a session
        should be added. If not, use the most recent session

        Parameters
        ----------
        force_new : bool
            If True, will not prompt for confirmation

        Returns
        -------
        Session
        """
        n: int = self.n_sessions()
        if not force_new and self._confirm_add_session():
            session: Session = Session(self, n + 1)
            self.dataset.refresh()
            return session
        else:
            return self.get_session(n)

    def get_session(self, session_id: int) -> Session:
        """Get the session with the session_id indicated

        Parameters
        ----------
        session_id : int

        Returns
        -------
        Session
            The session
        """
        assert session_id <= self.n_sessions()
        return Session(self, session_id)

    def n_sessions(self) -> int:
        """Return the number of sessions the subject has

        Returns
        -------
        int
            n sessions
        """
        if not self.path.exists():
            return 0
        return len([i for i in os.listdir(self.path) if "ses-" in i])

    def participant_data(self) -> pd.DataFrame:
        return pd.DataFrame(pd.Series(self.to_dict())).T

    def to_dict(self) -> Dict:
        return {
            f: (
                self.__getattribute__(f)
                if f != "name"
                else self.__getattribute__("participant_name")
            )
            for f in self.properties
        }

    def _confirm_add_session(self) -> bool:
        """Confirm that we want to add a session"""
        n_sessions: int = self.n_sessions()
        if n_sessions > 0:
            msgbox = QMessageBox()
            msgbox.setText(f"OK to start new session: #{n_sessions+1}?")
            ybtn = msgbox.addButton("OK", QMessageBox.YesRole)
            msgbox.addButton(
                f"No, use previous session #{n_sessions}", QMessageBox.NoRole
            )
            msgbox.exec_()
            if msgbox.clickedButton() == ybtn:
                return True
            else:
                return False

        else:
            return True

    def _repr_pretty_(self, s, cycle):
        return self.participant_data()

    @staticmethod
    def ensure_participant_id(id: Union[str, int]) -> str:
        retval: str
        if type(id) == int:
            retval = f"sub-{id:02}"
        elif type(id) == str:
            retval = id
            if retval[0:4] != "sub-":
                retval = f"sub-{retval}"

        return retval

    @property
    def bids_path(self) -> Path:
        return Path(self.dataset.layout.root)

    @property
    def properties(self) -> List[str]:
        return ["participant_id"] + list(self.participant_sidecar.keys())

    @property
    def participant_label(self) -> str:
        return self.participant_id[4:]

    @property
    def participant_sidecar(self) -> Dict:
        with open(self.participant_sidecar_filepath, "r") as f:
            return json.load(f)

    @property
    def participant_sidecar_filepath(self) -> Path:
        return self.bids_path.joinpath("participants.json")

    @property
    def path(self) -> Path:
        return self.bids_path.joinpath(self.participant_id)
