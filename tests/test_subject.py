import pytest
import shutil
import tempfile

from pathlib import Path
from libbids.clibbids import Dataset, Session, Subject  # type: ignore


# Test fixture for Subject class
class TestSubject:
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

        yield

        # Clean up the temporary test directory and files
        shutil.rmtree(self.test_dir)

    # Test the path() method of Subject
    def test_path_test(self) -> None:
        # Create a Subject object with index 1
        subject: Subject = Subject(
            self.dataset, dict(name="Jacob", participant_id="sub-01")
        )

        # Expected path: subject path + session id
        expected_path: Path = self.dataset.bids_dir / "sub-01"

        # Check if the calculated path matches the expected path
        assert subject.path == expected_path

    # Test the prefix() method of Subject
    def test_prefix(self) -> None:
        # Create a Subject object with index 2
        subject: Subject = Subject(self.dataset, dict(participant_id="02"))

        # Expected prefix: subject id + "_" + session id
        expected_prefix: str = "sub-02"

        # Check if the calculated prefix matches the expected prefix
        assert subject.id == expected_prefix

    # test the label
    def test_label(self) -> None:
        # Create a Subject object with index 3
        subject: Subject = Subject(self.dataset, dict(participant_id="sub-03"))

        # Check if the calculated prefix matches the expected prefix
        assert subject.get_participant_label() == "03"

    # Test adding a session
    def test_add_session(self) -> None:
        # Create a Subject object with index 4
        subject: Subject = Subject(
            self.dataset, dict(participant_id="sub-04", name="J'Dinklage Morgoon")
        )

        # Create a session on the subject
        session: Session = subject.add_session(silent=True)

        assert session is not None
