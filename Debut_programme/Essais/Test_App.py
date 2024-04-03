import time
import numpy as np
import pyqtgraph as pg

app = pg.mkQApp()

sec_per_day = 3600*24

class DateAxisItem(pg.AxisItem):
    def tickStrings(self, values, scale, spacing):
        return [time.strftime('%Y-%m-%d', time.localtime(x)) for x in values]

axitem = DateAxisItem(orientation='bottom')
axitem.setTickSpacing(major=sec_per_day, minor=sec_per_day/4)

pw = pg.PlotWidget(axisItems={'bottom': axitem})

x = np.arange(7) * sec_per_day
y = np.array([0.1, 0.2, -0.5, 0.8, -0.1, 0.7, 0.3])
pw.plot(x=x, y=y, symbol='o')

pw.show()
app.exec()