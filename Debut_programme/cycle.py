# -*- coding: utf-8 -*-
"""
Created on Mon Mar 25 15:12:48 2024

@author: croche

@dependencies:
    time
    measure
    instrumentation
        pyvisa
"""

from time import time, sleep
from importlib import reload
from measure import mesure
from instrumentation import instrument

import Variables


CHECK_VARIABLE = 'current_variables.py'


class Cycle:
    Id_cycle: int
    Temperature: float
    Parametres: dict[str, any]
    Mesures: dict[str, mesure]
    Instruments: dict[str, instrument]
    
    def __init__(self, instruments: dict[str, instrument], mesures: dict[str, mesure]) -> None:
        self.Instruments = instruments
        self.Mesures = mesures
        self.Id_cycle = 0
        self.Temperature = .0
        
        self.Parametres = {}
        
        # Initialisation des instuments et des mesures
        self.check_variable()
        print(f'Cycle : {self.Parametres}')
        self.Instruments['Source'].init('Current')
        #TODO initialisation des instruments et des mesures
        
        #self.Temperature = self.Calcul_Temp()
        
    def init_cycle(self, id_cycle: int) -> None:
        self.Id_cycle = id_cycle
        self.check_variable()
        #for inst in self.Instruments.get_items():
        #    inst.new_cycle()
        for meas in self.Mesures.values():
            meas.new_cycle()
        #TODO faire l'initialisation d'un cycle
        
    def run_cycle(self) -> None:
        #TODO Changer la pàc des fonctions time en datetime (cf google)
        timer_0 = time()
        while self.Temperature >= 44.0:
            timer_1 = time()
            Continue = True
            while timer_1-timer_0 <= int(self.Parametres['Minutes_Prises'])*60 and Continue:
                time.sleep(0.02)
                timer_1 = time()
                #TODO la fonction qui permet à chaque instant de checker la température au sein de l'étuve
                if self.Temperature < 44.0:
                    Continue = False
            if Continue:
                timer_0 = timer_1
                self.get_mesures()
                
        while self.Temperature <= 45.0:
            Continue = True
            while timer_1-timer_0 <= int(self.Parametres['Minutes_Prises'])*60 and Continue:
                time.sleep(0.02)
                timer_1 = time()
                #TODO la fonction qui permet à chaque instant de checker la température au sein de l'étuve
                if self.Temperature > 45.0:
                    Continue = False
            timer_0 = timer_1
            self.get_mesures()
        print('')
        
    def run_cycle_(self) -> None:

        sleep(10)
        self.Mesures['Van_der_Pauw_elem1'].get_mesure()
        self.Mesures['Van_der_Pauw_elem1'].write_on_file(self.Id_cycle)
        
    def get_mesures(self) -> None:
        for meas in self.Mesures:
            meas.get_mesures()
            meas.write_on_file(self.Id_cycle)
            
    def check_variable(self):
        # Reload le module
        reload(Variables)
        # Regarde si l'on doit changer les variables
        if Variables.Changement_Variable:
            new_variables_file = Variables.Path_current_variable
            # Regarde dans le fichiers précisé les nouvelles valeurs de variables
            with open(new_variables_file, 'r') as f:
                for i in f.readlines():
                    i = i.replace('\n', '')
                    if i != '': # Cas des sauts de lignes (pas prise en compte)
                        # Change les variables de chaque Objets en fonction de sa classe
                        (nom, val) = i.split('\t')
                        (type_, glob, param) = nom.split('-')
                    print(type_, glob, param)
            #TODO pensez à ajouter pour chaque objet un moyen de modifier si nécessaire les valeurs des param
                    if type_ == 'M':
                        self.Mesures[glob].Parametres[param] = val
                    elif type_ == 'C':
                        self.Parametres[param] = val
                    elif type_ == 'I':
                        self.Instruments[glob].Parametres[param] = val
            # Change le fichier de check pour ne pas changer les variables à chaque cycle
            if False:
                with open('Variables.py', 'r') as f:
                    lines = f.readlines()
                with open('Variables.py', 'w') as f:
                    for l in lines:               
                        if l == lines[0]:
                            f.writelines('Changement_Variable = False\n')
                        else:
                            f.writelines(l)