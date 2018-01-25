import sys
sys.setrecursionlimit(2000)

from setuptools import setup

APP = ['pyfab_app.py']
DATA_FILES = []
OPTIONS = {'argv_emulation': True,
 'includes': ['sip', 'PyQt5']}

setup(
app=APP,
data_files=DATA_FILES,
options={'py2app': OPTIONS},
setup_requires=['py2app'],
)