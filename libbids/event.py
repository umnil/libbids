import threading
import numpy as np
from datetime import timedelta
from typing import (
    Any,
    Dict,
    Iterator,
    List,
    Optional,
    Union,
    cast,
)


class Event:
    def __init__(
        self,
        onset: Optional[Union[timedelta, float]] = None,
        duration: Optional[Union[timedelta, float]] = None,
        trial_type: Optional[str] = None,
        triggerable: bool = False,
    ):
        """Create an event to be recorded or delivered during a task

        Parameters
        ----------
        onset : Optional[Union[timedelta, float]]
            The time since the begining of the run until this event
        duration : Optional[Union[timedelta, float]]
            The duration of the event
        trial_type : Optional[str]
            The event label
        triggerable : bool
            Whether the event can be stopped prematurely; i.e., duration can be
            shortened based on experimental events in real-time
        """
        self.onset: Optional[timedelta] = (
            timedelta(seconds=cast(float, onset))
            if type(onset) not in [type(None), timedelta]
            else cast(Optional[timedelta], onset)
        )
        self.duration: Optional[timedelta] = (
            timedelta(seconds=cast(float, duration))
            if type(duration) not in [type(None), timedelta]
            else cast(Optional[timedelta], duration)
        )
        self.trial_type: Optional[str] = trial_type
        self.triggerable: bool = triggerable
        self.threading_event: threading.Event = threading.Event()

    def __iter__(self) -> Iterator:
        keys: List[str] = ["onset", "duration", "trial_type"]
        for key in keys:
            value = self.__getattribute__(key)
            if isinstance(value, timedelta):
                yield (key, value.total_seconds())
            else:
                yield (key, value)

    def __repr__(self) -> str:
        return str(self.to_dict())

    def is_set(self) -> bool:
        return self.threading_event.is_set()

    def set(self) -> None:
        if self.triggerable:
            self.threading_event.set()

    def to_dict(self) -> Dict[str, Any]:
        return {k: v for k, v in self}

    @property
    def is_timed(self) -> bool:
        return (self.onset is not None) and (self.duration is not None)

    @classmethod
    def sample_onsets(cls, events: List["Event"], sfreq: int) -> np.ndarray:
        """Generate an array of sample indices that correlate to the onset of
        each event listed in events

        Parameters
        ----------
        events : List[Event]
            List of events to convert to sample onsets
        sfreq : int
            The sampling frequency used to convert from seconds to samples

        Returns
        -------
        np.ndarray
            The array of sample onsets
        """
        time_onsets: np.ndarray = np.array(
            [cast(timedelta, e.onset).total_seconds() for e in events]
        )
        return time_onsets * sfreq

    @classmethod
    def generate_fixed_duration_events(
        cls, duration: timedelta, trial_types: List[str], task_duration: timedelta
    ) -> List["Event"]:
        """Generates a list of events. Each event will have a fixed width. The
        number of events returned will be enough to fill up the `task_duration`

        Parameters
        ----------
        duration : timedelta
            The duration that each event should last
        trial_types : List[str]
            A unique list of event names.
        task_duration : timedelta
            The total duration of the task

        Returns
        -------
        List[Event]
            A list of events that will define the task, or part of a task
        """
        n_events: int = int(task_duration.total_seconds() // duration.total_seconds())
        return [
            Event(
                onset=timedelta(seconds=i * duration.total_seconds()),
                duration=duration,
                trial_type=trial_types[i % len(trial_types)],
            )
            for i in range(n_events)
        ]

    @classmethod
    def generate_variable_duration_events(
        cls,
        min_duration: timedelta,
        max_duration: timedelta,
        trial_types: List[str],
        task_duration: timedelta,
    ) -> List["Event"]:
        max_n_events: int = int(
            task_duration.total_seconds() // min_duration.total_seconds()
        )
        shift: float = min_duration.total_seconds()
        scale: float = max_duration.total_seconds() - min_duration.total_seconds()
        event_durations_array: np.ndarray = np.random.rand(max_n_events) * scale + shift
        event_durations: List[timedelta] = [
            timedelta(seconds=s) for s in event_durations_array
        ]
        event_onsets_array: np.ndarray = [0] + np.cumsum(
            [i.total_seconds() for i in event_durations]
        ).tolist()[:-1]
        event_onsets: List[timedelta] = [
            timedelta(seconds=d) for d in event_onsets_array
        ]
        n_types: int = len(trial_types)
        n_repeats: int = int(np.ceil(max_n_events / n_types))
        if n_types > 2:
            list_events: List = []
            type_set: np.ndarray = np.random.default_rng().choice(
                trial_types, size=n_types, replace=False
            )
            list_events.append(type_set.tolist())
            for _ in range(n_repeats - 1):
                type_set = np.random.default_rng().choice(
                    trial_types, size=n_types, replace=False
                )
                # Loop ensures that types do not immediately repeat
                while type_set[0] == list_events[-1][-1]:
                    type_set = np.random.default_rng().choice(
                        trial_types, size=n_types, replace=False
                    )
                list_events.append(type_set.tolist())
            event_types: np.ndarray = np.hstack(list_events)
        else:
            event_types = np.array(trial_types * n_repeats)
        return [
            Event(onset, duration, trial_type)
            for onset, duration, trial_type in zip(
                event_onsets, event_durations, event_types
            )
        ]
