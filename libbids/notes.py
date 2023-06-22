import json
import pandas as pd  # type: ignore
from pathlib import Path
from typing import Dict, List, Optional, TYPE_CHECKING

if TYPE_CHECKING:
    from .task import Task


class Notes:
    def __init__(self, task: "Task"):
        """A Notes object for managing notes about a given task

        Parameters
        ----------
        task : Task
            The task object that these notes will belong to
        """
        self.task: Task = task
        self._validate()

    def _validate(self) -> None:
        """Validate that these notes are good. This process includes:
        - Ensuring that there is an associated JSON side car
        - This side car must describe the columns in the notes table,
          including the mandatory run id column
        - Any previously existing TSV file where notes are stored must
          *not* have a column that is *not* found in the JSON file. Any
          surplus columns in the JSON file will automatically be appended
          to the TSV table
        """
        assert (
            self.sidecar_path.exists()
        ), "A side car for notes is required for using notes"

        expected_columns: List[str] = self.columns
        observed_columns: List[str] = list(self.table.colunms)
        assert (
            "run" in expected_columns
        ), "A notes table should have a column of run ids"
        for c in observed_columns:
            assert (
                c in expected_columns
            ), f"'{c}' is a column that is not described in the notes sidecar"

    def add_note(self, run_id: int, note_data: Dict, overwrite: bool = False):
        """Add a note to the notes table

        Parameters
        ----------
        run_id : int
            The id of the run associated with the note
        note_data : Dict
            A dictionary of information to add to the notes table
        overwrite : bool
            If true the data will overwrite existing information. If false, an
            exception will be raised if data already exists
        """
        for key in note_data:
            assert key in self.columns, f"'{key}' not a recognized note info"

        note_data.update({"run": run_id})
        if self.get_note(run_id) is not None:
            if not overwrite:
                raise Exception(f"Data for run id {run_id} already exists")

            idx: int = self.table.query(f"run == {run_id}").index
            self.table.iloc[idx, :] = pd.Series(note_data)
            return

        self.table = pd.concat(
            [self.table, pd.Series(note_data)], ignore_index=True, axis=0
        )

    def get_note(self, run_id: int) -> Optional[Dict]:
        """Retrives a note about a run

        Paramters
        ---------
        run_id : int
            The ID of the note

        Returns
        -------
        Optional[Dict]
            Returns the dictionary of the note, or None, if no note is found
        """
        result: pd.DataFrame = self.table[self.table["run"] == run_id]
        if result.shape[0] == 0:
            return None
        return result.to_dict()

    @property
    def columns(self) -> List[str]:
        return list(self.sidecar.keys())

    @property
    def path(self) -> Path:
        filename: str = self.prefix + ".tsv"
        return self.task.modality_path / filename

    @property
    def prefix(self) -> str:
        return "_".join([self.task.prefix, "notes"])

    @property
    def sidecar(self) -> Dict:
        with open(self.sidecar_path, "r") as fh:
            return json.load(fh)

    @property
    def sidecar_path(self) -> Path:
        filename: str = self.prefix + ".json"
        return self.task.modality_path / filename

    @property
    def table(self) -> pd.DataFrame:
        if self.path.exists():
            return pd.read_csv(self.path, sep="\t")
        else:
            return pd.DataFrame(columns=self.columns)

    @table.setter
    @property
    def table(self, value: pd.DataFrame):
        pd.to_csv(self.path, sep="\t", index=False)
