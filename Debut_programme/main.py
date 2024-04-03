# -*- coding: utf-8 -*-
"""
Created on Thu Mar 21 16:24:36 2024

@author: croche

@dependencies:
    intrumentation
        pyvisa
    measure
    cylce
        time
    
    
Mesurer pour un nombre de cycle définis les résistivités de la couche d'or
pour des paliers de températures
Par la méthode de Van der Pauw avec le switch ()
Programme pour le projet GYTRIX SLB

"""

import instrumentation as inst
import measure as meas
import cycle

# Paramètres du programme principale

ADRESS_SWITCH = 11
ADRESS_SOURCE = 26


# Programme principale
if __name__ == "__main__":
    Instruments = {
        'Switch' : None, #inst.SWITCH_KEITHLEY(ADRESS_SWITCH),
        'Source' : inst.SOURCE_KEITHLEY(ADRESS_SOURCE)
    }
    
    Measures = {
        'Van_der_Pauw_elem1' : meas.Van_der_Pauw(Instruments['Source'], Instruments['Switch'], 'Van_der_Pauw_elem1'),
        #'Van_der_Pauw_elem2' : meas.Van_der_Pauw(Instruments['Source'], Instruments['Switch'], 'Van_der_Pauw_elem2'),
        #'Van_der_Pauw_elem3' : meas.Van_der_Pauw(Instruments['Source'], Instruments['Switch'], 'Van_der_Pauw_elem3'),
        #'Thermo_Couple_Platine' : meas.Thermocouple_Platine(Instruments['Source'], Instruments['Switch'], 'Thermo_Couple_Platine')
    }
    
    
    # Création du cycle et initialisation des instuments et des mesures à partie des variables du fichier
    cycle = cycle.Cycle(Instruments, Measures)

    for id_cycle in range(1, 11):
        print(f"Début du cycle n° {id_cycle}")
        cycle.init_cycle(id_cycle)
        cycle.run_cycle_()
        print(f"Fin du cycle n° {id_cycle}")
    
    print("C'est finis ! ")
    
    
    
    
