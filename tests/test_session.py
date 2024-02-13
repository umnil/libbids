import pytest
import shutil
import tempfile

from pathlib import Path
from libbids import Dataset
from libbids.clibbids import Session, Subject  # type: ignore
from typing import cast


# Test fixture for Session class
class TestSession:
    # Set up the test fixture
    @pytest.fixture(autouse=True)
    def setup(self):
        self.test_dir = Path(tempfile.mkdtemp())
        self.participants_file = self.test_dir / "participants.tsv"
        self.participants_sidecar_file = self.test_dir / "participants.json"
        (self.test_dir / "sub-01").mkdir()
        (self.test_dir / "sub-02").mkdir()

        # Create participants table file
        with open(self.participants_file, "w") as file:
            file.write("participant_id\tname\n")
            file.write("sub-01\tJohn\n")
            file.write("sub-02\tAlice\n")

        # Create participants sidecar file
        with open(self.participants_sidecar_file, "w") as file:
            file.write("{\n")
            file.write('\t"name": {\n')
            file.write('\t\t"Description": "Name of participant"\n')
            file.write("\t}\n")
            file.write("}\n")

        self.dataset = Dataset(self.test_dir, True)
        self.subject = self.dataset.get_subject(1)

        yield

        # Clean up the temporary test directory and files
        shutil.rmtree(self.test_dir)

    # Test the path() method of Session
    def test_path_test(self) -> None:
        # Create a Session object with index 1
        session: Session = Session(self.subject, 1)

        # Expected path: subject path + session id
        expected_path: Path = cast(Subject, self.subject).path / "ses-01"

        # Check if the calculated path matches the expected path
        assert session.path == expected_path

    # Test the prefix() method of Session
    def test_prefix(self) -> None:
        # Create a Session object with index 2
        session: Session = Session(self.subject, 2)

        # Expected prefix: subject id + "_" + session id
        expected_prefix: str = cast(Subject, self.subject).id + "_" + session.id

        # Check if the calculated prefix matches the expected prefix
        assert session.prefix == expected_prefix

    # test the label
    def test_label(self) -> None:
        # Create a Session object with index 3
        session: Session = Session(self.subject, 3)

        # Check if the calculated prefix matches the expected prefix
        assert session.label == "03"

    # test padding
    def test_padding(self) -> None:
        # Create a Session object with index 4
        session: Session = Session(self.subject, 4)

        # Check if the calculated prefix matches the expected prefix
        assert session.padding == 2

    # test index
    def test_index(self) -> None:
        # Creat a Session object with index 5
        session: Session = Session(self.subject, 5)

        # Check if the calculated index matches
        assert session.index == 5
