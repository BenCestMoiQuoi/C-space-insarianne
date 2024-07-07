# -*- coding: utf-8 -*-
"""
Created on Wed Mar 27 09:25:24 2024

@author: croche
"""

import pyqtgraph as pg
from PyQt6 import QtWidgets, QtCore
import datetime
import time


class Application(QtWidgets.QMainWindow):
    Widget_one: QtWidgets.QWidget
    Widget_two: QtWidgets.QWidget

    Button: QtWidgets.QPushButton

    plot_graph1: pg.PlotWidget
    plot_graph2: pg.PlotWidget

    timer: QtCore.QTimer

    Time: list[float]
    Temp: list[float]
    Res_Time: list[float]
    Res_Temp: list[float]

    fichiers: list[str]
    nb_fichiers: int
    Line_1: list[any]
    Line_2: list[any]
    
    
    
    def __init__(self, fichiers: list[str]) -> None:
        # Initialisationde le fenêtre
        super().__init__()

        self.fichiers = fichiers
        self.nb_fichiers = len(self.fichiers)

        # Mise en place d'un Widget central pouvant acceuillir plusieurs autres Widgets
        layout = QtWidgets.QVBoxLayout()
        central_widget = QtWidgets.QWidget()
        central_widget.setLayout(layout)
        self.setCentralWidget(central_widget)

        # Création des Widgets de Plot
        self.plot_graph1 = pg.PlotWidget()
        self.plot_graph2 = pg.PlotWidget()
        # Création du Widget Button
        self.Button = QtWidgets.QPushButton('Démarrer Visualisation')
        self.Button.clicked.connect(self.beg_up)

        # Ajout au Widget central des Plots et du Button
        layout.addWidget(self.plot_graph1)
        layout.addWidget(self.plot_graph2)
        layout.addWidget(self.Button)

        # Définition de la figure 1 (Résistance surfacique en fonction du Temps)
        self.plot_graph1.setTitle('Résistance Surfacique vs Temps', size='20pt')
        self.plot_graph1.setLabel('left', 'Résistance Surfacique (Ohm.m)')
        self.plot_graph1.setLabel('bottom', 'Temps (h)')
        # Définition de la figure 2 (Résistance surfacique en fonction de la Température)
        self.plot_graph2.setTitle('Résistance Surfacique vs Température', siz='20pt')
        self.plot_graph2.setLabel('left', 'Résistance Surfacique (Ohm.m)')
        self.plot_graph2.setLabel('bottom', 'Température (°C)')


        # Affichage de la fenêtre
        self.show()

        # Déffinition des variables pour les figures
        self.Time = []
        self.Temp = []
        self.Res_Time = []
        self.Res_Temp = []
        
        self.Line_1 = []
        self.Line_2 = []
        
        for i in range(self.nb_fichiers):
            self.Time.append([])
            self.Temp.append([])
            self.Res_Time.append([])
            self.Res_Temp.append([])
            self.Line_1.append(self.plot_graph1.plot(self.Res_Time[i], self.Time[i], 
                                                     name=self.fichiers[i],
                                                     pen=pg.mkPen(color=(255, 0, 0)),
                                                     symbolBrush='r'))
            self.Line_2.append(self.plot_graph2.plot(self.Res_Temp[i], self.Temp[i], 
                                                     name=self.fichiers[i],
                                                     pen=pg.mkPen(color=(0, 0, 255)),
                                                     symbolBrush='b'))


        # Définition d'un Timer pour rafraichir les valeurs
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.update)


    def beg_up(self) -> None:
        self.update()
        self.Button.clicked.connect(QtWidgets.QApplication.quit)
        # Début du timmer de 1 min
        self.timer.start(60*1000)


    def update(self) -> bool:
        print('update')
        
        for i in range(self.nb_fichiers):
            self.Res_Temp[i] = []
            self.Res_Time[i] = []
            self.Temp[i] = []
            self.Time[i] = []
            nom_fichier = f'DATA/{self.fichiers[i]}.txt'
            with open(nom_fichier, 'r') as f:
                lines = f.readlines()
            print(lines)
            if len(lines) <= 3:
                return False
            for l in range(3, len(lines)):
                print(l)
                line = lines[l].replace('\n', '').split('\t')
                print(line[3])
                self.Res_Temp[i].append(float(line[3]))
                self.Res_Time[i].append(float(line[3]))
                self.Temp[i].append(float(line[3]))
                
                #♠self.Time[i].append(time.strptime(line[2], '%Y-%m-%d %H:%M:%S'))
                #TODO Fonction pour le time
                self.Time[i].append(time.mktime(datetime.datetime.strptime(line[2], '%Y-%m-%d %H:%M:%S').timetuple()))
                

            self.Line_1[i].setData(self.Time[i], self.Res_Time[i])
            self.Line_2[i].setData(self.Temp[i], self.Res_Temp[i])
            return True
        
        
        

if __name__ == '__main__':
    fichiers = ['Van_der_Pauw_elem1',
                #'Van_der_Pauw_elem2',
    ]

    app = QtWidgets.QApplication([])
    Window = Application(fichiers)
        
    app.exec()