import sbnw
# load the model
model = sbnw.loadsbml('../testcases/BorisEJB.xml')

# retrieve layout information
layout = model.layout
# access the api
network = layout.network
canvas = layout.canvas

# seed node coordinates randomly
network.randomize(canvas)
# apply the FR-algorithm
network.autolayout()

# remove a node
network.removenode(network.nodes[0])

# fit to a window
layout.fitwindow(0,0,300,300)