from pathlib import Path
from typing import TYPE_CHECKING
from clibbids import Entity

if TYPE_CHECKING:
    from .subject import Subject


class Session(Entity):
    def __init__(self, subject: "Subject", idx: int):
        """Initialize a new session for a given subject

        Parameters
        ----------
        idx : int
            The id of the session
        subject_path : Path
            Path to the subject directory that owns this sessions
        """
        super(Session, self).__init__("Session", value=idx)
        self.subject: "Subject" = subject
        self.idx: int = idx
        self.path.mkdir(exist_ok=True)

    @property
    def path(self) -> Path:
        return self.subject.path.joinpath(self.id)

    @property
    def prefix(self) -> str:
        return "_".join([self.subject.id, self.id])
