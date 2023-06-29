import numpy as np  # type: ignore

from datetime import timedelta
from pathlib import Path
from typing import Any, List, Optional, TYPE_CHECKING, cast

from .clibbids import Entity
from .event import Event

if TYPE_CHECKING:
    from .task import Task


class Run(Entity):
    def __init__(
        self,
        task: "Task",
    ):
        """Initializes the objects necessary to run a training data collection
        session

        Parameters
        ----------
        task : Task
            The task object that this run will exectute
        """
        super(Run, self).__init__("Run", value=task.n_runs + 1)
        self.task: "Task" = task

        self.current_event: Optional[Event] = None
        self.remaining_events: List[Event] = self.task.events

    def append_event(self, event: Event):
        """Append the event data to the event table

        Parameters
        ----------
        event : Event
            The event data to save
        """
        line: str = (
            "\t".join([str(x) for x in cast(List[Any], dict(event).values())]) + "\n"
        )
        self.eventbuf.writelines([line])

    def end_current_event(self) -> None:
        """Finishes out the current event"""
        if self.current_event is not None:
            current_event: Event = cast(Event, self.current_event)
            current_event_onset: timedelta = cast(timedelta, current_event.onset)
            current_event.duration = self.elapsed_time - current_event_onset
            self.current_event = current_event
            self.task.on_event_end(self.current_event)
            self.previous_event = self.current_event
            self.current_event = None

    def initialize_event_file(self) -> None:
        """Initializes the event file for writing"""
        self.eventbuf = open(self.event_filepath, "w")
        header: str = "\t".join(["onset", "duration", "trial_type"]) + "\n"
        self.eventbuf.writelines([header])

    def is_current_event_finished(self) -> bool:
        """Determines whether the current event is complete"""
        if (self.current_event is not None) and (
            self.current_event.duration is not None
        ):
            current_event: Event = cast(Event, self.current_event)
            current_event_onset: timedelta = cast(timedelta, current_event.onset)
            current_event_duration: timedelta = cast(timedelta, current_event.duration)
            current_event_end_time: timedelta = (
                current_event_onset + current_event_duration
            )
            if self.elapsed_time >= current_event_end_time:
                return True

        return False

    def is_next_event_ready(self) -> bool:
        """Determines if the next event should begin"""
        if self.next_event.onset is None:
            return self.next_event.is_set()
        else:
            return self.elapsed_time >= self.next_event.onset

    def pop_event(self) -> Event:
        event: Event = self.remaining_events[0]
        self.remaining_events = self.remaining_events[1:]
        return event

    def start(self) -> None:
        self.initialize_event_file()
        for ins in self.task.instruments:
            ins.start(self.task.id, self.id)
        self.remaining_events = self.task.events
        self.n_samples: int = 0

        # Determine current event
        self.current_event = None
        if self.elapsed_time == self.next_event.onset:
            self.current_event = self.pop_event()
            self.task.on_event_start(self.current_event)

        # Throw away any samples collected during setup
        self.task.process()
        while not self.done:
            # Handle events
            if self.is_current_event_finished():
                self.end_current_event()

            if (len(self.remaining_events) > 0) and self.is_next_event_ready():
                self.end_current_event()
                self.current_event = self.pop_event()
                current_event: Event = cast(Event, self.current_event)
                current_event.onset = self.elapsed_time
                self.current_event = current_event
                self.task.on_event_start(current_event)

            # Handle sampling
            sample: np.ndarray = self.task.process()
            self.n_samples += sample.shape[-1]

        # Final event
        self.end_current_event()

        # Final sample
        self.task.process(True)

        self.stop()

    def stop(self):
        for ins in self.task.instruments:
            ins.stop()
        self.eventbuf.close()

    @property
    def done(self) -> bool:
        if self.task.duration is None:
            return len(self.remaining_events) == 0
        else:
            return self.elapsed_time >= self.task.duration

    @property
    def elapsed_time(self) -> timedelta:
        elapsed_seconds: float = self.n_samples / self.sfreq
        return timedelta(seconds=elapsed_seconds)

    @property
    def event_filepath(self) -> Path:
        event_filename: str = "_".join([self.prefix, "events.tsv"])
        return self.task.modality_path.joinpath(event_filename)

    @property
    def next_event(self) -> Event:
        return self.remaining_events[0]

    @property
    def prefix(self) -> str:
        return "_".join([self.subject_id, self.task.session.id, self.task.id, self.id])

    @property
    def sfreq(self) -> int:
        return self.task.primary_instrument.sfreq

    @property
    def subject_dir(self) -> Path:
        return self.task.session.subject.path

    @property
    def subject_id(self) -> str:
        return self.subject_dir.name
