# -*- coding: utf-8 -*-
"""
Created on Thu Mar 21 16:49:22 2024

@author: croche
"""

from instrumentation import GPIB
import numpy as np
import time as time
import matplotlib.pyplot as plt
import math as m
import scipy.optimize as opt
import datetime as date

#import application as app


    
class mesure:
    """
    Classe mère pour ce qui concerne les mesures
    Permet de gérer leur initialisation, l'écriture dans les fichiers et la
    gestion des nouveaux cycles
    """
    id_mesure: int
    name: str

    appareil: dict[str, GPIB]
    Parametres: dict[str, any]
    
    def __init__(self, name: str = ""):
        self.id_mesure = 0
        self.name = name
        self.appareil = {}
        print("oui")

    def write_on_file(self, ajout_line: str, flag: str = None, flag_line: str = ''):
        """
        Permet à l'ensemble des classe filles d'écrire dans un fichier les résultats.
        Si il y a un flag d'activer, mettre {id_cycle}_{id_mesure} dans la variable :flag:
        """
        path_file = f'DATA/{self.name}.txt'
        with open(path_file, 'a') as f:
            f.write(ajout_line)
        if flag is not None:
            path_file = f'DATA/{self.name}/{flag}.txt'
            with open(path_file, 'w') as f:
                f.write(flag_line)
    
    def new_cycle(self):
        self.id_mesure = -1




class Van_der_Pauw(mesure):
    name: str
    id_mesure: int
    Rs: float
    Rv_moy: float
    Rh_moy: float
    all_R: list[float]
    delta_Rv: float
    delta_Rh: float
    Temp: float
    flag: bool

    appareil: dict[str, GPIB]
    Parametres: dict[str, any]

    mode: str

    def __init__(self, Source: GPIB, Switch: GPIB, name: str) -> None:
        super().__init__(name)
        self.appareil["Source"] = Source
        self.appareil["Switch"] = Switch
        
        self.Rs = .0
        self.Rv_moy = .0
        self.Rh_moy = .0
        self.all_R = []
        self.delta_Rv = .0
        self.delta_Rh = .0
        self.Temp = .0
        self.flag = False
        
        self.Parametres = {}
        
        
        
    def fonction(self, Rs):
        return np.exp(-np.pi*self.Rv_moy/Rs)+np.exp(-np.pi*self.Rh_moy/Rs)-1


    def get_mesure(self, temp: float = .0) -> None:
        self.Rs = .0
        self.Rv_moy = .0
        self.Rh_moy = .0
        self.all_R = []
        self.delta_Rv = .0
        self.delta_Rh = .0
        self.Temp = temp
        self.flag = True
        self.id_mesure += 1
        
        # Début des 8 mesures
        self.all_R.append(self.get_R_WXYZ('A', 'B', 'C', 'D'))
        self.all_R.append(self.get_R_WXYZ('B', 'A', 'D', 'C'))
        self.all_R.append(self.get_R_WXYZ('C', 'D', 'A', 'B'))
        self.all_R.append(self.get_R_WXYZ('D', 'C', 'B', 'A'))
        self.Rv_moy = (self.all_R[0]+self.all_R[1]+self.all_R[2]+self.all_R[3])/4
        # Calcul de l'incertitude
        self.delta_Rv = m.sqrt((
                        (self.all_R[0]-self.Rv_moy)**2+(self.all_R[1]-self.Rv_moy)**2+
                        (self.all_R[2]-self.Rv_moy)**2+(self.all_R[3]-self.Rv_moy)**2)/4)
        
        self.all_R.append(self.get_R_WXYZ('A', 'C', 'D', 'B'))
        self.all_R.append(self.get_R_WXYZ('C', 'A', 'B', 'D'))
        self.all_R.append(self.get_R_WXYZ('D', 'B', 'A', 'C'))
        self.all_R.append(self.get_R_WXYZ('B', 'D', 'C', 'A'))
        self.Rh_moy = (self.all_R[4]+self.all_R[5]+self.all_R[6]+self.all_R[7])/4
        # Calcul de l'incertitude
        self.delta_Rh = m.sqrt((
                        (self.all_R[4]-self.Rh_moy)**2+(self.all_R[5]-self.Rh_moy)**2+
                        (self.all_R[6]-self.Rh_moy)**2+(self.all_R[7]-self.Rh_moy)**2)/4)
        # :flag: si un problème est détecté avec l'incertitude
        if self.delta_Rh/self.Rh_moy >= 0.03 or self.delta_Rv/self.Rv_moy >= 0.03:
            self.flag = False
        
        self.Rs = opt.fsolve(self.fonction, np.pi*(self.Rv_moy+self.Rh_moy)/(2*np.log(2)))[0]


    def get_R_WXYZ(self, W: str, X: str,Y: str, Z:str) -> float:
        # W = connecté au Hi du keithley
        # X = connecté au Lo du keithley
        # Y = connecté au Sense High
        # Z = connecté au Sense Low
        
        doors = [f'11{self.Parametres[f'Position_Switch_{W}']}',
                 f'12{self.Parametres[f'Position_Switch_{X}']}',
                 f'13{self.Parametres[f'Position_Switch_{Y}']}',
                 f'14{self.Parametres[f'Position_Switch_{Z}']}',
                 ]        
        self.appareil['Switch'].close_doors(doors)


        #TODO faire la fonction pour le mesurometre qui renvoie les 5 valeurs de résistance
        """
        if self.Parametres["Mode"] == 'IV':
            # initialisation de la mesure
            i_sence = np.linspace(0,float(self.Parametres['Current_Max']),int(self.Parametres['Nb_Point_IV']))
            vds = []
            ids = []
            
            self.appareil['Source'].set_output_on()
            
            for i in i_sence:
                # Réalisation des différentes mesures
                i_, v_ = self.mesure_current(i)
                ids.append(i_)
                vds.append(v_)
                
            self.appareil['Source'].set_output_off()
        """
        retour = self.appareil['Source'].query("Envoie fonction(self.Parametres['Nb_Point_IV'])")

        vds = retour[0:self.Parametres['Nb_Point_IV']]
        ids = retour[self.Parametres['Nb_Point_IV']:2*self.Parametres['Nb_Point_IV']]

        if self.Parametres['Plot_IV'] == 'True':
            print(vds, ids)
            plt.figure()
            plt.plot(vds, ids)
            plt.xlabel('Current (A)')
            plt.ylabel('Voltage (V)')
            plt.title(f'Get_R_{W}{X}{Y}{Z}')
            plt.draw()
                
        # Renvoie du premier coef de la regression linéaire
        return np.polyfit(ids, vds, 1)[0]



    def mesure_current(self, i: float = 0) -> tuple[float, float]:
        
        self.appareil['Source'].set_output_amps(i)
        self.appareil['Source'].set_output_on()
        ids = self.appareil['Source'].measure_current()
        vds = self.appareil['Source'].measure_voltage()
        self.appareil['Source'].set_output_off()
        return ids, vds

    def write_on_file(self, id_cycle: int) -> None:
        txt = f"{id_cycle}\t{self.id_mesure}\t{date.datetime.today().strftime('%Y-%m-%d %H:%M:%S')}\t{self.Rs}\t{self.Rv_moy}\t{self.Rh_moy}\t"
        
        txt += f'{self.delta_Rv}\t{self.delta_Rh}\t{self.Temp}\t{self.flag}\n'
        if not self.flag:
            txt_flag = f"{id_cycle}\t{self.id_mesure}\t{date.datetime.today().strftime('%Y-%m-%d %H:%M:%S')}\t{self.Rs}\t{self.Rv_moy}\t{self.Rh_moy}\t"
            for i in self.all_R:
                txt += f'{i}'
                if i != self.all_R[-1]:
                    txt += '--'
                else:
                    txt += '\t'        
            super().write_on_file(txt, f'{id_cycle}_{self.id_mesure}', txt_flag)
        else:
            super().write_on_file(txt)



class Thermocouple_Platine(mesure):
    name: str
    id_mesure: int
    Temperature: float
    
    appareil: dict[str, GPIB]
    Parametres: dict[str, any]
    
    def __init__(self, Source: GPIB, Switch: GPIB, name: str) -> None:
        super().__init__(name)
        self.appareil["Source"] = Source
        self.appareil["Switch"] = Switch
        
        self.Temperature = .0
        

                
                
        