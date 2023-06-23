import re
import numpy as np
from abc import abstractmethod
from datetime import timedelta
from pathlib import Path
from typing import Dict, List, Optional, TYPE_CHECKING
from clibbids import Entity
from .event import Event
from .instruments import Instrument
from .notes import Notes
from .run import Run

if TYPE_CHECKING:
    from clibbids import Session


class Task(Entity):
    def __init__(
        self,
        session: "Session",
        name: str,
        instruments: List[Instrument],
        events: List[Event],
        duration: Optional[timedelta] = None,
    ):
        """Initialize a task for a session. The Task is responsible for managing
        the events and timing. It defines these properties and callbacks that
        the run will use to execute the task.

        Parameters
        ----------
        session : Session
            The session that owns this task
        name : str
            Name of the task
        instruments: List[Instrument]
            A list of instruments used for data collection and stimulus
            delivery.

            **NOTE**: The first instrument in the list must be the sampling
            instrument that timing of the task will be used to keep time as data
            is sampled
        events : List[Event]
            A list of predefined events to run through the task
        duration : Optional[timedelta]
            The duration of the task. Note if this is not specified, the task
            will run until all events are exhausted
        """
        super(Task, self).__init__("Task", "task", name)
        self.session: "Session" = session
        self.instruments: List[Instrument] = instruments
        self.events: List[Event] = events
        self.duration: Optional[timedelta] = duration

    def add_note(self, run_id: int, note_data: Dict):
        """Adds a note to a task-associated note TSV file

        Parameters
        ----------
        run_id : int
            The index of the run associated with the note
        note_data : Dict
            A dictionary of information to add to the note
        """
        notes: Notes = Notes(self)
        notes.add_note(run_id, note_data)

    def add_run(self) -> Run:
        """Create a run associated with this taks for the current session

        Returns
        -------
        Run
            A run object that can be used to start the session
        """
        return Run(self)

    @abstractmethod
    def on_event_start(self, event: Event):
        """Allows the task to do something when a new event is encountered

        Parameters
        ----------
        event : Event
        """
        raise Exception("Not Implemented")

    @abstractmethod
    def on_event_end(self, event: Event):
        """Called when the event changes

        Parameters
        ----------
        event : Event
            The event that just finished
        """
        raise Exception("Not Implemented")

    @abstractmethod
    def process(self, *args, **kwargs) -> np.ndarray:
        """This method is to be implemented by inheriting classes to define how
        the task processes through time. This method should also handle
        sampling data from the primary instrument and return the sampled data.
        The run that executes this task will use this as the clock for the
        task.

        Returns
        -------
        np.ndarray
            An array of sampled data from the primary instrument. The number of
            samples in the time dimension must be the last dimension if the
            array is multidimension, e.g., channels being the first dimension
        """
        raise Exception("Not Implemented")

    @property
    def prefix(self) -> str:
        return "_".join([self.session.prefix, self.id])

    @property
    def primary_instrument(self) -> Instrument:
        return self.instruments[0]

    @property
    def modality_path(self) -> Path:
        return self.primary_instrument.modality_path

    @property
    def n_runs(self) -> int:
        pattern: str = (
            self.id + "_run-[0-9]{" + str(self.padding) + "}.*\.edf"  # noqa: W605
        )
        run_files: List[Path] = [
            f
            for f in self.modality_path.iterdir()
            if re.search(pattern, f.name) is not None
        ]
        run_pattern: str = "run-([0-9]{" + str(self.padding) + "}).*"
        run_matches_potential: List[Optional[re.Match]] = [
            re.search(run_pattern, f.name) for f in run_files
        ]
        run_matches: List[re.Match] = [
            m for m in run_matches_potential if m is not None
        ]
        run_numbers: List[int] = [int(m.groups()[0]) for m in run_matches]
        return len(run_numbers)
