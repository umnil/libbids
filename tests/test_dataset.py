import pytest
import tempfile
import shutil
from pathlib import Path
from libbids import Dataset


class TestDataset:
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

    def test_add_subject_valid_arguments_returns_subject(self):
        args = {"participant_id": "sub-03", "name": "Bob"}

        subject = self.dataset.add_subject(args)

        assert subject is not None
        assert subject["participant_id"] == "sub-03"
        assert subject["name"] == "Bob"

    def test_add_subject_too_many_arguments_returns_nullopt(self):
        args = {"participant_id": "sub-03", "name": "Bob", "age": "25"}

        with pytest.raises(Exception):
            self.dataset.add_subject(args)

    def test_get_subject_existing_subject_returns_subject(self):
        subject = self.dataset.get_subject(1)

        assert subject is not None
        assert subject["participant_id"] == "sub-01"
        assert subject["name"] == "John"

    def test_get_subject_nonexistent_subject_returns_nullopt(self):
        subject = self.dataset.get_subject(3)

        assert subject is None

    def test_is_subject_existing_subject_returns_true(self):
        result = self.dataset.is_subject(2)

        assert result is True

    def test_is_subject_nonexistent_subject_returns_false(self):
        result = self.dataset.is_subject(3)

        assert result is False

    def test_get_subjects_returns_subject_ids(self):
        subject_ids = self.dataset.get_subjects()

        assert len(subject_ids) == 2
        assert subject_ids[0] == 1
        assert subject_ids[1] == 2
