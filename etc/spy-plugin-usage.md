The network editor/viewer shows a 2D representation of the model, with species depicted
by nodes and reactions depicted by curves. The `nwed` module is the interface 
used to display a visual representation of an SBML model:

```
sbmlstr='''<?xml version=...'''

import nwed
nwed.setsbml(sbmlstr)
```

If the SBML model has layout information, it will be used for specifying node 
coordinates. If there is no preexisting layout, the viewer will generate one 
automatically using the Fruchterman-Reingold algorithm.

The model displayed in the viewer may be saved to a file using the `Save As`
menu command or converted to a raw SBML string by calling another method, `getsbml`:

```
import nwed
outsbml = nwed.getsbml()
```