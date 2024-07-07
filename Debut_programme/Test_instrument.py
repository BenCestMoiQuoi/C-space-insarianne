# -*- coding: utf-8 -*-
"""
Created on Tue Mar 26 11:05:45 2024

@author: croche
"""

import instrumentation as inst
import time
import keyboard as kb
import numpy as np
import matplotlib.pyplot as plt

from PyQt6 import QtGui, QtWidgets, QtCore
import pyqtgraph as pg

Adresse_Source = 26
Adresse_Switch = 16

I = 0.500 # A

def mesure():
    source.set_output_on()
    print(f'{source.measure_current()} A')
    print(f'{source.measure_voltage()} V')
    source.set_output_off()
    
def mesure_IV():
    switch.close_doors(['1101', '1202', '1303', '1404'])
    source.set_output_on()
    
    i_sence = -np.linspace(I/10,I,50)
    vds = np.zeros(np.size(i_sence))
    ids = np.zeros(np.size(i_sence))
    
    x = 0
    for i in i_sence:
        # Réalisation des différentes mesures
        source.set_output_amps(i)
        time.sleep(0.1)
        ids[x] = source.measure_current()
        vds[x] = source.measure_voltage()
        x+=1
    
    source.set_output_off()
    switch.open_doors(['1101', '1202', '1303', '1404'])
    
    poly = np.polyfit(ids, vds, 1)
    I_ = np.linspace(0, np.max(ids), 100)
    V_ = poly[0]*I_+poly[1]
    
    rds = vds/ids
    plt.figure(1)
    plt.plot(vds, ids, 'o')
    plt.plot(V_, I_, '-')
    plt.xlabel('Current (A)')
    plt.ylabel('Voltage (V)')
    plt.draw()
    
    plt.figure(2)
    plt.plot(ids, rds, '+')
    plt.plot(ids,0*ids+poly[0])
    print(poly)
    
    return ids, vds, rds
        
    
    

if __name__ == '__main__':
    source = inst.SOURCE_KEITHLEY(Adresse_Source)
    switch = inst.SWITCH_KEITHLEY(Adresse_Switch)
    
    app = QtWidgets.QApplication([])   
    win = QtWidgets.QMainWindow() # creates a window

    # Mise en place d'un Widget central pouvant acceuillir plusieurs autres Widgets
    layout = QtWidgets.QVBoxLayout()
    central_widget = QtWidgets.QWidget()
    central_widget.setLayout(layout)
    win.setCentralWidget(central_widget)

    # Créations des widgets de polt
    p0 = pg.PlotWidget()
    p1 = pg.PlotWidget()
    layout.addWidget(p0)
    layout.addWidget(p1)
    
    p0.setTitle('Resistance')  # creates empty space for the plot in the window
    p0.setLabel(axis='left', text='R', units='Ohm')
    p0.setLabel(axis='bottom', text='Current', units='A')
    curve0 = p0.plot()    # create an empty "plot" (a curve to plot)

    p1.setTitle('Bias')  # creates empty space for the plot in the window
    p1.setLabel(axis='left', text='U', units='V')
    p1.setLabel(axis='bottom', text='Current', units='A')
    curve1 = p1.plot()    # create an empty "plot" (a curve to plot)
    
    source.set_source_type_current() # Set current source
    source.set_voltage_compliance('1') # Set maximum voltage 1V
    source.write("smua.sense=1")    # Set 4_Wire sense
    
    ids, vds, rds = mesure_IV()
    
    curve0.setData(ids,rds) 
    curve1.setData(ids,vds)
    app.processEvents()
