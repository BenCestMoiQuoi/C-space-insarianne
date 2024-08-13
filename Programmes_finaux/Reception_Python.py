#################################################
#                                               #
#               Programme Python                #
#                                               #
#################################################
#
#       Nom : Reception_Python.py
#       Auteurs : Roche Corentin (@BenCestMoiQuoi)
#                 Léo-Paul You (@Lyouu)
#       Version : 2
#       Date : 07/07/2024
#       Der_modif : 13/08/2024
#
#################################################
#
#   Programme qui a pour but d'observer les données 
#       qui circule sur un port COM avec la carte au sol
#       et l'écrit sur un fichier de l'ordinateur

## Import Librairies ##

import serial

## Variables ##

PORT_COM = 'COM4'
VITESSE = 9600

FICHIER = 'DATA/Mesure.txt'

## Programme Principale ##

if __name__ == "__main__":
    ser = serial.Serial(PORT_COM, VITESSE, timeout = 1)
    try:
        while 1:
            line = ser.readline()
            if line: 
                with open(FICHIER,'a') as f:
                    try:
                        f.write(line.decode())
                    except:
                        print("impossible")
    except KeyboardInterrupt:
        ser.close()
            
        