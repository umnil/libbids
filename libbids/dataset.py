"""
Supplemental pybids object for manipulating the dataset
"""
import os
import sys
import json
import pandas as pd  # type: ignore
from bids.layout import BIDSLayout  # type: ignore
from pathlib import Path
from typing import Dict, List, Optional, cast
from .clibbids import Subject

try:
    from PyQt6.QtWidgets import QMessageBox, QWidget  # type: ignore
except ModuleNotFoundError:
    from PyQt5.QtWidgets import QMessageBox, QWidget  # type: ignore


class Dataset:
    def __init__(self, bids_dir: Path, silent: bool = False):
        """Initialize a BIDSDataset

        Parameters
        ----------
        bids_dir : Path
            Path to the root directory of the BIDS layout
        """
        self.bids_dir: Path = bids_dir
        self.layout: BIDSLayout = BIDSLayout(str(self.bids_dir), validate=False)
        self.silent: bool = silent

    def add_subject(self, *args, **kwargs) -> Optional[Subject]:
        """add a subject to the dataset. the arguments passed to this function
        are defined by the JSON sidecar of participants

        Returns
        -------
        Optional[Subject]
            If the subject is successfully added, the subject object is
            returned, else None
        """
        assert len(args) + len(kwargs) <= len(
            self.participants_properties
        ), "Too many arguments"
        if len(args) > 0:
            keys: List[str] = self.participants_properties[: len(args)]
            kwargs.update({k: v for k, v in zip(keys, args)})

        if (not self.silent) and (
            not self._confirm_add_subject(kwargs["participant_id"], kwargs["name"])
        ):
            return self.get_subject(kwargs["participant_id"])

        subject = Subject(self, kwargs)
        if subject.get_participant_label() not in self.layout.get_subjects():
            self.append_participant(subject)

        os.makedirs(
            os.path.join(self.layout.root, subject.get_participant_id()), exist_ok=True
        )
        self.layout = BIDSLayout(self.layout.root, validate=False)
        return subject

    def append_participant(self, subject: Subject):
        """Add the given subject to our participants table

        Parameters
        ----------
        subject : Subject
            The subject object that has our subject's data
        """
        fn: str = "participants.tsv"
        fp: str = os.path.join(self.layout.root, fn)
        subject_data: dict = subject.to_dict()
        subject_table: pd.DataFrame = pd.DataFrame(pd.Series(subject_data))
        participant_data: pd.DataFrame = pd.concat(
            [self.participant_table, subject_table], ignore_index=True
        )
        participant_data.to_csv(fp, sep="\t", index=False)

    def get_subject(self, idx: int) -> Optional[Subject]:
        """Given a subject's index, get that subject or None if not found

        Parameters
        ----------
        idx : int
            The id of the subject

        Returns
        -------
        Optional[Subject]
        """
        participant_id: str = Subject.ensure_participant_id(idx)
        query = self.participant_table.query(f"participant_id == '{participant_id}'")
        if query.shape[0] < 1:
            return None

        data_dict: Dict = {k: str(v) for k, v in query.iloc[0, :].to_dict().items()}
        return Subject(self, data_dict)

    def get_subjects(self) -> List[int]:
        """Retreive a list of subject ids

        Returns
        -------
        List[int]
        """
        participant_ids: List[str] = self.participant_table["participant_id"].values.tolist()
        return [
            int(ids[4:])
            for ids in participant_ids
        ]

    def is_subject(self, idx: int) -> bool:
        """determins if the subject with the provided id exists

        Parameters
        ----------
        idx : int
            subject integer

        Returns
        -------
        bool
            Whether the subject exists
        """
        return self.get_subject(idx) is not None

    def refresh(self):
        """The lawyout needs to be refereshed when changes are made"""
        self.layout = BIDSLayout(self.layout.root, validate=False)

    def _confirm_add_subject(self, subject_idx: int, subject_name: str) -> bool:
        """Confirm that the subject should be added

        Parameters
        ----------
        subject_idx : int
            The id of the subjec to add
        subject_name : str
            The name of the subject. If this is not a new subject, we double
            check the name to ensure there is no overwriting

        Returns
        -------
        bool
            Whether to add the subject or not
        """
        if not self.is_subject(subject_idx):
            mb = QMessageBox.warning(
                cast(QWidget, None),
                "Warning",
                "New Participant ID\nPlease confirm that this is a new participant",
                QMessageBox.StandardButton.Ok,
                QMessageBox.StandardButton.Cancel,
            )
            if mb == QMessageBox.StandardButton.Cancel:
                return False
            else:
                return True
        else:
            subject: Subject = cast(Subject, self.get_subject(subject_idx))
            assert subject_name == subject["name"], (
                f"The participant's name ({subject_name}) "
                f"doesn't match the id ({subject['name']})"
            )
            return False

    @property
    def participants_properties(self) -> List[str]:
        return ["participant_id"] + list(self.participants_sidecar.keys())

    @property
    def participants_filepath(self) -> Path:
        return self.bids_dir.joinpath("participants.tsv")

    @property
    def participants_sidecar(self) -> Dict:
        return json.load(open(self.participants_sidecar_filepath, "r"))

    @property
    def participants_sidecar_filepath(self) -> Path:
        return self.bids_dir.joinpath("participants.json")

    @property
    def participant_table(self) -> pd.DataFrame:
        participant_query: List = self.layout.get(
            suffix="participants", extension="tsv"
        )
        if len(participant_query) < 1:
            return pd.DataFrame(columns=self.participants_properties)

        return pd.read_csv(self.participants_filepath, sep="\t")
