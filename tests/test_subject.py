import pytest
import shutil
import tempfile

from pathlib import Path
from libbids import Dataset
from libbids.clibbids import Session, Subject

# Test fixture for Subject class
class TestSubject:
    @pytest.fixture(autouse=True)

    # Set up the test fixture
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
            file.write('\t}\n')
            file.write("}\n")

        self.dataset = Dataset(self.test_dir, True)

        yield

        # Clean up the temporary test directory and files
        shutil.rmtree(self.test_dir)

    def test_one(self):
        raise Exception("Unimplemented Tests")
