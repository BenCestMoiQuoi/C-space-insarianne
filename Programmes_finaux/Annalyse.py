# -*- coding: utf-8 -*-
"""
Created on Sun Jul 07 18:04:24 2024

@author: croche
"""

from PyQt6 import QtWidgets
from Applications import Application

if __name__ == '__main__':
    
    app = QtWidgets.QApplication([])
    Win = Application('save.txt')
    
    app.exec()