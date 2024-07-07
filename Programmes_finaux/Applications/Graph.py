from Applications.Mother import Graph, NB_LINE_FILE_DECO, \
    COL_ACCX, COL_GYROX, COL_COUNT_S, COL_ALTI, COL_PRES, COL_TEMP


class Graph_3Acc(Graph):
    fichier: str
    
    nb_graph: int
    X: list[list[float]]
    Y: list[list[float]]

    Line: list[any]


    def __init__(self, fichier: str) -> None:
        self.fichier = fichier
        
        color = [(255, 0, 0), (0, 255, 0), (0, 0, 255)]
        brush = ['r', 'g', 'b']
        label = ['AccX', 'AccY', 'AccZ']

        super().__init__(color, brush, label)

        # Définition de la figure 1 (Température de la cuve en fonction du Temps)
        self.setTitle('Accélération vs Temps', size='20pt')
        self.setLabel('left', 'Accélération (m/s^2)')
        self.setLabel('bottom', 'Temps (s)')

    def update(self) -> None:
        for i in range(self.nb_graph):
            self.Y[i] = []
            self.X[i] = []
            nom_fichier = f'DATA/{self.fichier}.txt'
            with open(nom_fichier, 'r') as f:
                lines = f.readlines()
            if len(lines) <= NB_LINE_FILE_DECO:
                return False
            for l in range(NB_LINE_FILE_DECO, len(lines)):
                line = lines[l].replace('\n', '').split('\t')
                
                self.Y[i].append(float(line[COL_ACCX+i]))
                self.X[i].append((int(line[COL_COUNT_S])*1000+int(line[COL_COUNT_S+1]))/1000)

            self.Line[i].setData(self.X[i], self.Y[i])



class Graph_3Gyr(Graph):
    fichier: str
    
    nb_graph: int
    X: list[list[float]]
    Y: list[list[float]]

    Line: list[any]


    def __init__(self, fichier: str) -> None:
        self.fichier = fichier
        
        color = [(255, 0, 0), (0, 255, 0), (0, 0, 255)]
        brush = ['r', 'g', 'b']
        label = ['GyrX', 'GyrY', 'GyrZ']

        super().__init__(color, brush, label)

        # Définition de la figure 1 (Température de la cuve en fonction du Temps)
        self.setTitle('Rotation vs Temps', size='20pt')
        self.setLabel('left', 'Rotation (°C/s)')
        self.setLabel('bottom', 'Temps (s)')

    def update(self) -> None:
        for i in range(self.nb_graph):
            self.Y[i] = []
            self.X[i] = []
            nom_fichier = f'DATA/{self.fichier}.txt'
            with open(nom_fichier, 'r') as f:
                lines = f.readlines()
            if len(lines) <= NB_LINE_FILE_DECO:
                return False
            for l in range(NB_LINE_FILE_DECO, len(lines)):
                line = lines[l].replace('\n', '').split('\t')
                
                self.Y[i].append(float(line[COL_GYROX+i]))
                self.X[i].append((int(line[COL_COUNT_S])*1000+int(line[COL_COUNT_S+1]))/1000)

            self.Line[i].setData(self.X[i], self.Y[i])
            


class Graph_Pre(Graph):
    fichier: str
    
    nb_graph: int
    X: list[list[float]]
    Y: list[list[float]]

    Line: list[any]


    def __init__(self, fichier: str) -> None:
        self.fichier = fichier
        
        color = [(255, 0, 0)]
        brush = ['r']
        label = ['Pression']

        super().__init__(color, brush, label)

        # Définition de la figure 1 (Température de la cuve en fonction du Temps)
        self.setTitle('Pression vs Altitude', size='20pt')
        self.setLabel('left', 'Pression (Bar)')
        self.setLabel('bottom', 'Altitude (m)')

    def update(self) -> None:
        for i in range(self.nb_graph):
            self.Y[i] = []
            self.X[i] = []
            nom_fichier = f'DATA/{self.fichier}.txt'
            with open(nom_fichier, 'r') as f:
                lines = f.readlines()
            if len(lines) <= NB_LINE_FILE_DECO:
                return False
            for l in range(NB_LINE_FILE_DECO, len(lines)):
                line = lines[l].replace('\n', '').split('\t')
                
                self.Y[i].append(float(line[COL_PRES]))
                self.X[i].append(float(line[COL_ALTI]))

            self.Line[i].setData(self.X[i], self.Y[i])
            

class Graph_Temp(Graph):
    fichier: str
    
    nb_graph: int
    X: list[list[float]]
    Y: list[list[float]]

    Line: list[any]


    def __init__(self, fichier: str) -> None:
        self.fichier = fichier
        
        color = [(0, 255, 0)]
        brush = ['b']
        label = ['Température']

        super().__init__(color, brush, label)

        # Définition de la figure 1 (Température de la cuve en fonction du Temps)
        self.setTitle('Température vs Altitude', size='20pt')
        self.setLabel('left', 'Température (°C)')
        self.setLabel('bottom', 'Altitude (m)')

    def update(self) -> None:
        for i in range(self.nb_graph):
            self.Y[i] = []
            self.X[i] = []
            nom_fichier = f'DATA/{self.fichier}.txt'
            with open(nom_fichier, 'r') as f:
                lines = f.readlines()
            if len(lines) <= NB_LINE_FILE_DECO:
                return False
            for l in range(NB_LINE_FILE_DECO, len(lines)):
                line = lines[l].replace('\n', '').split('\t')
                
                self.Y[i].append(float(line[COL_TEMP]))
                self.X[i].append(float(line[COL_ALTI]))

            self.Line[i].setData(self.X[i], self.Y[i])



class Graph_Alti(Graph):
    fichier: str
    
    nb_graph: int
    X: list[list[float]]
    Y: list[list[float]]

    Line: list[any]


    def __init__(self, fichier: str) -> None:
        self.fichier = fichier
        
        color = [(255, 0, 0)]
        brush = ['r']
        label = ['Altitude']

        super().__init__(color, brush, label)

        # Définition de la figure 1 (Température de la cuve en fonction du Temps)
        self.setTitle('Altitude vs Temps', size='20pt')
        self.setLabel('left', 'Altitude (m)')
        self.setLabel('bottom', 'Temps (s)')

    def update(self) -> None:
        for i in range(self.nb_graph):
            self.Y[i] = []
            self.X[i] = []
            nom_fichier = f'DATA/{self.fichier}.txt'
            with open(nom_fichier, 'r') as f:
                lines = f.readlines()
            if len(lines) <= NB_LINE_FILE_DECO:
                return False
            for l in range(NB_LINE_FILE_DECO, len(lines)):
                line = lines[l].replace('\n', '').split('\t')
                
                self.Y[i].append(float(line[COL_ALTI]))
                self.X[i].append((int(line[COL_COUNT_S])*1000+int(line[COL_COUNT_S+1]))/1000)

            self.Line[i].setData(self.X[i], self.Y[i])