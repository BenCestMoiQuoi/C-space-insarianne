import pyqtgraph as pg

COL_NB_PACKET = 0
COL_ACCX = 1
COL_GYROX = 4
COL_TEMP = 7
COL_PRES = 8
COL_ALTI = 9
COL_COUNT_S = 10
COL_COUNT_MS = 11

NB_LINE_FILE_DECO = 3


class Graph(pg.PlotWidget):
    nb_graph: int
    X: list[list[float]]
    Y: list[list[float]]

    Line: list[any]

    def __init__(self, color: list[tuple[int]] = [(255, 0, 0)], brush: list[str] = ['r'], label: list[str] = ['']) -> None:
        super().__init__()

        self.nb_graph = len(color)

        # Déffinition des variables pour les figures
        self.X = []
        self.Y = []
        
        self.Line = []
        
        for i in range(self.nb_graph):
            self.X.append([])
            self.Y.append([])
            self.Line.append(self.plot(self.Y[i], self.X[i],
                               name=label[i],
                               pen=pg.mkPen(color=color[i]),
                               symbolBrush=brush[i]))
            
    def update(self) -> None:
        pass