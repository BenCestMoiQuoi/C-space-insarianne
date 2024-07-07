from PyQt6 import QtWidgets, QtCore

from Applications.Mother import Graph

from Applications.Graph import Graph_3Acc, Graph_3Gyr, Graph_Pre, \
                               Graph_Temp, Graph_Alti



class Application(QtWidgets.QMainWindow):
    fichiers: dict[str, list[str]]
    graphs: list[Graph]

    timer: QtCore.QTimer
    Button: QtWidgets.QPushButton

    state: bool
    
    Acc3: Graph
    Gyro3: Graph
    Pre: Graph
    Temp: Graph
    Alti: Graph


    def __init__(self, fichier) -> None:
        # Initialisationde le fenêtre
        super().__init__()
        self.fichier = fichier
        self.state = False

        self.graphs = []

        # Définition d'un Timer pour rafraichir les valeurs
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.update)

        self.resize(1800, 900)

        self.init_app()
        print('fin_init')
        self.show()
        


    def init_app(self) -> None:
        print('debut_init')
        self.Acc3 = Graph_3Acc(self.fichier)
        self.Gyro3 = Graph_3Gyr(self.fichier)
        self.Pre = Graph_Pre(self.fichier)
        self.Temp = Graph_Temp(self.fichier)
        self.Alti = Graph_Alti(self.fichier)
        
        print('fin init_ graph')
        
        self.graphs.append(self.Acc3)
        self.graphs.append(self.Gyro3)
        self.graphs.append(self.Pre)
        self.graphs.append(self.Temp)
        self.graphs.append(self.Alti)


        # Création et mise en premier plan (centrale) de la page
        layout = QtWidgets.QGridLayout()
        central_widget = QtWidgets.QWidget()
        central_widget.setLayout(layout)
        self.setCentralWidget(central_widget)

        # Création du Widget Button
        self.Button = QtWidgets.QPushButton('Démarrer Visualisation')
        self.Button.clicked.connect(self.Button_clicked)

        # Ajout de chaque Widget
        layout.addWidget(self.Acc3, 0, 0, 1, 2)
        layout.addWidget(self.Gyro3, 1, 0, 1, 2)
        layout.addWidget(self.Pre, 2, 0)
        layout.addWidget(self.Temp, 2, 1)
        layout.addWidget(self.Alti, 0, 2, 3, 1)
        
        layout.addWidget(self.Button, 3, 2)
        print('init_app')


    def Button_clicked(self) -> None:
        if not self.state:
            self.update()
            self.state = not self.state
            self.Button.setText("Pause Emulation")
            # Début du timmer de 1 min
            self.timer.start(60*1000)
        else:
            self.state = not self.state
            self.Button.setText("Début Emulation")
            # Stop du timmer
            self.timer.stop()

    def update(self) -> None:
        for i in self.graphs:
            i.update()
