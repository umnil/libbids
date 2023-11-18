try:
    from PyQt6.QtWidgets import QMessageBox, QPushButton  # type: ignore
except ModuleNotFoundError:
    from PyQt5.QtWidgets import QMessageBox, QPushButton # type: ignore


def qprompt(msg: str, yes_button_text: str = "Yes", no_button_text: str = "No"):
    """Display a QMessageBox

    Parameters
    ----------
    msg : str
        The message to display
    yes_button_text : str
        The text for the yes button
    no_button_text : no
        The text for the no button

    Return
    ------
    bool
        Whether the yes button was clicked
    """
    # Adjust messagebox class for PyQt6
    if not hasattr(QMessageBox, "YesRole"):
        for role in QMessageBox.ButtonRole:
            setattr(QMessageBox, role.name, role)
            setattr(QMessageBox, "exec_", QMessageBox.exec)

    msgbox: QMessageBox = QMessageBox()
    msgbox.setText(msg)
    yes_button: QPushButton = msgbox.addButton(yes_button_text, QMessageBox.YesRole)
    msgbox.addButton(no_button_text, QMessageBox.NoRole)
    QMessageBox.exec_(msgbox)
    return msgbox.clickedButton() == yes_button
