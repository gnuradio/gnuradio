from .. views.projects import ProjectsView

class ProjectsController(object):
    def __init__(self):

        # Load the main view class and initialize QMainWindow
        self._view = ProjectsView()

    def get_view(self):
        return self._view
