# -*- coding: utf-8 -*-
#################################################
#                                               #
#               Programme Python                #
#                                               #
#################################################
#
#       Nom : Analyse.py
#       Auteurs : Roche Corentin (@BenCestMoiQuoi)
#                 Léo-Paul You (@Lyouu)
#       Version : 2
#       Date : 07/07/2024
#       Der_modif : 13/08/2024
#
#################################################
#
#   Programme qui a pour but de visualiser les données 
#       de la fusée à partir d'un fichiers où elles sont stockées
#   Possibilité de moduler le programme dans le dossier "Applications"

## Include librairies ##

from PyQt6 import QtWidgets
from Applications import Application

## Variable ##

NOM_FICHIER = 'Mesure'

## Programme principale ##

if __name__ == '__main__':
    
    app = QtWidgets.QApplication([])
    Win = Application(NOM_FICHIER)
    
    app.exec()