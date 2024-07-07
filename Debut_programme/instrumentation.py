# -*- coding: utf-8 -*-
"""
Created on Thu Mar 21 16:29:32 2024

@author: croche

@dependencies:
    pyvisa
    
"""

import pyvisa as visa
# import serial
import time


class instrument:
    Type_appareil: str
    
    def __init__(self, type_app: str = '') -> None:
        self.Type_appareil = type_app
        
    def __repr__(self) -> str:
        return f"Appareil de type : {self.Type_appareil}"

class GPIB(instrument):
    Address_GPIB: int
    Type_appareil: str
    Inst: visa.ResourceManager
    
    
    def __init__(self, adress: int = 0, type_app: str = '', time_out: int = 120000) -> None:
        super().__init__(type_app)
        self.Address_GPIB = adress
        
        # Initialisation de la liaison
        rm = visa.ResourceManager()
        self.Inst = rm.open_resource("GPIB0::%s" % self.Address_GPIB)
        self.Inst.timeout = time_out
        
        print(self)
        self.reset()
        
    def __repr__(self) -> str:
        return f"{super().__repr__()}\nAdresse GPIB de l'appareil : {self.Address_GPIB}\n{self.idn()}"
    
    def getSuccess(self) -> bool:
        return self.Inst.read(0.1)

    def query(self, mes, delay=1) -> str:
        return self.Inst.query(mes, delay)

    def write(self, mes) -> None:
        self.Inst.write(mes)

    def close(self) -> None:
        self.Inst.clear()
        self.Inst.close()

    def idn(self) -> str:
        return self.query("*IDN?")
        
    def reset(self) -> None:
        """ resets instrument """
        self.write("reset()")

    
    
class SWITCH_KEITHLEY(GPIB):
    Address_GPIB: int
    Type_appareil: str
    Inst: visa.ResourceManager
    Parametres: dict[str, str]
    
    def __init__(self, adress: int = 0) -> None:
        super().__init__(adress, "Switch Keithley")
        self.Parametres = {}
        
    def init(self) -> None:
        self.reset()
        
    def reset(self) -> None:
        self.write('channel.reset("slot1")')
        
    def close_door(self, door: str) -> None:
        self.write(f'channel.close("{door}")')
        
    def close_doors(self, doors: list[str] = []) -> None:
        txt_doors = ''
        for i in doors:
            txt_doors += i
            if i != doors[-1]:
                txt_doors += ', '
            self.close_door(txt_doors)

    def open_door(self, door: str) -> None:
        self.write(f'channel.open("{door}")')

    def open_doors(self, doors: str) -> None:
        txt_doors = ''
        for i in doors:
            txt_doors += i
            if i != doors[-1]:
                txt_doors += ', '
            self.open_door(txt_doors)
        
class SOURCE_KEITHLEY(GPIB):
    """
    Classe qui nous permet de gérer les evenements liéer au Sourcemetre lors de la 
    manipe, dans notre cas c'est le Keithley K2601
    """
    Address_GPIB: int
    Type_appareil: str
    Inst: visa.ResourceManager
    Parametres: dict[str, str]
    
    def __init__(self, adress: int=0) -> None:
        super().__init__(adress, "Current_Voltage_Keithley")
        self.Parametres = {}
        
    def init(self, type: str) -> None:
        if type == 'Current':
            self.set_source_type_current()
            print(self.Parametres)
            self.set_voltage_compliance(self.Parametres['Voltage_Max'])
            self.write('smua.sense=1')
#            self.set_wire_sense(self.Parametres['Number_Wire'])
        elif type == 'Voltage':
            self.set_source_type_voltage()
            self.set_current_compliance(self.Parametres['Current_Max'])
            self.write('smua.sense=1')
            #self.set_wire_sense(self.Parametres['Number_Wire'])

    def measure_current(self, channel: str = 'a') -> float:
        """ queries instrument, returns value """
        if channel == 'a' or channel == 'b':
            return float(self.query(f"print(smu{channel}.measure.i())"))
        else:
            raise ValueError("Invalid channel sent to function measure_current")

    def measure_voltage(self, channel: str = 'a') -> float:
        """ queries instrument, returns value """
        if channel == 'a' or channel == 'b':
            return float(self.query(f"print(smu{channel}.measure.v())"))
        else:
            raise ValueError("Invalid channel sent to function measure_voltage")

    def measure_resistance(self, channel: str = 'a') -> float:
        """ queries instrument, returns value """
        if channel == 'a' or channel == 'b':
            return float(self.query(f"print(smu{channel}.measure.r())"))
        else:
            raise ValueError("Invalid channel sent to function measure_voltage")     

    def reset_channel(self, channel: str = 'a') -> None:
        """ resets VISA channel """
        if channel == 'a' or channel == 'b':
            self.write(f"smu{channel}.reset()")
        else:
            raise ValueError("Invalid channel sent to function reset_channel")

    def set_filter_on(self, filter_count: str, filter_type: str, channel: str='a') -> None:
        """ queries instrument """
        if channel=='a' or channel == 'b':
            self.write(f"smu{channel}.measure.filter.count = {filter_count}")
            if filter_type == 'REAPEAT_AVG' or filter_type == 'MEDIAN' or filter_type == 'MOVING_AVG':
                self.write(f"smu{channel}.measure.filter.type = smu{channel}.FILTER_{filter_type}")
            else:
                raise ValueError("Invalid filter_type sent to function set_filter_on")
            self.write(f"smu{channel}.measure.filter.enable = smu{channel}.FILTER_ON")
        else:
            raise ValueError("Invalid channel sent to function set_filter_on") 
            
    def set_filter_off(self, channel: str = 'a') -> None:
        """ queries instrument """
        if channel == 'a' or channel == 'b':
            self.write(f"smu{channel}.measure.filter.enable = smu{channel}.FILTER_OFF")
        else:
            raise ValueError("Invalid channel sent to function set_filter_off") 


    def set_measure_current_range(self, meas_range: str=None, channel: str='a') -> None:
        """ if *meas_range* is not specified, measurement will be set to autorange """
        if channel == 'a' or channel == 'b':
            if meas_range is None:
                self.write(f"smu{channel}.measure.autorangei = smu{channel}.AUTORANGE_ON")
            else:                         
                self.write(f"smu{channel}.measure.rangei = {meas_range}")
        else:
            raise ValueError("Invalid channel sent to function set_measure_current_range")
    
    def set_NPLC(self, NPLC: int=1, channel: str='a') -> str:
        """ queries instrument """
        if channel == 'a' or channel == 'b':
            return self.write(f"smu{channel}.measure.nplc = {NPLC}")
        else:
            raise ValueError("Invalid channel sent to function set_NPLC")   

    def set_current_compliance(self, compliance: str, channel: str='a') -> None:
        """ Set max output current level """
        if channel == 'a' or channel == 'b':
            self.write(f"smu{channel}.source.limiti = {compliance}")
        else:
            raise ValueError("Invalid channel sent to function set_current_compliance")

    def set_voltage_compliance(self, compliance: str, channel: str='a') -> None:
        """ Set max output current level """
        if channel == 'a' or channel == 'b':
            self.write(f"smu{channel}.source.limitv = {compliance}")
        else:
            raise ValueError("Invalid channel sent to function set_current_compliance")

    def set_output_on(self, channel: str='a') -> None:
        """ turn on output """
        if channel == 'a' or channel == 'b':
            self.write(f"smu{channel}.source.output = smu{channel}.OUTPUT_ON")
        else:
            raise ValueError("Invalid channel sent to function set_output_on")

    def set_output_off(self, channel: str='a') -> None:
        """ turn off output """
        if channel == 'a' or channel == 'b':
            self.write(f"smu{channel}.source.output = smu{channel}.OUTPUT_OFF")
        else:
            raise ValueError("Invalid channel sent to function set_output_off")

    def set_output_amps(self, amps: float, channel: str='a') -> None:
        """ set the output current in amps """
        if channel == 'a' or channel == 'b':
            self.write(f"smu{channel}.source.leveli = {amps}")
        else:
            raise ValueError("Invalid channel sent to function set_output_amps")

    def set_output_volts(self, volts: float, channel: str='a') -> None:
        """ set the output voltage level in volts """
        if channel == 'a' or channel == 'b':
            self.write(f"smu{channel}.source.levelv = {volts}")
        else:
            raise ValueError("Invalid channel sent to function set_output_volts")

    def set_source_type_current(self, src_range: str=None, channel: str='a') -> None:
        """
        set the smu channel to source current (in amps)
        if `src_range` is not specified, smu source will be set to autorange
        """
        if channel == 'a' or 'b':
            self.Inst.write(f"smu{channel}.source.func = smu{channel}.OUTPUT_DCAMPS")
            if src_range is None:
                self.write(f"smu{channel}.source.autorangei = smu{channel}.AUTORANGE_ON")
            else:
                self.write(f"smu{channel}.source.rangev = {src_range}")
        else:
            raise ValueError("Invalid channel sent to function set_source_type_current")

    def set_source_type_voltage(self, src_range: str=None, channel: str='a') -> None:
        """
        set the smu channel to source volts
        if `src_range` is not specified, smu source will be set to autorange
        """
        if channel == 'a' or channel == 'b':
            self.write(f"smu{channel}.source.func = smu{channel}.OUTPUT_DCVOLTS")
            if src_range is None:
                self.write(f"smu{channel}.source.autorangev = smu{channel}.AUTORANGE_ON")
            else:
                self.write(f"smu{channel}.source.rangev = {src_range}")
        else:
            raise ValueError("Invalid channel sent to function set_source_type_voltage")
    
    def set_wire_sense(self, nb_wire: int=2, channel: str='a') -> None:
        """
        Set the smu sense wire
        `nb_wire` is 2 or 4
        """
        if channel == 'a' or channel == 'b':
            possible = {2: '0', 4:'1'}
            if nb_wire in possible.keys():
                self.write(f'smu{channel}.sense={possible[nb_wire]}')
            else:
                raise ValueError("Invalid nb_wire sent to the function set_wire_sense")
        else:
            raise ValueError("Invalid channel sent to function set_wire_sense")

"""

class Serie(instrument):
    port: str
    baudrate: int
    ser: serial.Serial
    
    def __init__(self, port: str, baudrate: int = 9600, type_app: str = '') -> None:
        super().__init__(type_app)
        self.port = port
        self.baudrate = baudrate
        self.ser = serial.Serial(self.port, self.baudrate)
    
    def close(self) -> None:
        self.ser.close()
        
    def __repr__(self) -> str:
        return f"{super().__repr__()}\nPort COM de l'appareil : {self.port}\nVitesse de transmition : {self.baudrate}"
    

class Arduino(Serie):
    port: str
    baudrate: int
    ser: serial.Serial
    pin: dict[str, str | int]
    
    INIT: int = 50
    SELECT_CHANNEL: int = 100
    
    def __init__(self, port: str, baudrate: int = 9600, pin: dict[str, int | str] = {}) -> None:
        super().__init__(port, baudrate, 'Arduino')
        
        self.ser.write(chr(self.INIT))
        if (self.ser.read(1) != self.INIT):
            print('Arduino non initialisé')
            while(1):
                time.sleep(1)
        print('Arduino initialisé')
        
    def select_channel(self, en: int=0, an0: int=0, an1: int=0) -> None:
        self.ser.write(chr(self.SELECT_CHANNEL))
        self.ser.write(chr(en))
        self.ser.write(chr(an0))
        self.ser.write(chr(an1))

"""