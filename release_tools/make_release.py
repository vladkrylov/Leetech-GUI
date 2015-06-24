## Example of usage:
#  python make_release.py
#      /d/TBONE/Arm/Qt_interfase/build-one_motor_raw_ss-Desktop_Qt_5_2_1_MinGW_32bit-Release/release/one_ motor_raw.exe
#      /c/Qt/5.2.1/5.2.1/mingw48_32/bin

import os
import shutil

from argparse import ArgumentParser
from subprocess import call

parser = ArgumentParser(description='Makes a release of current Qt project as a standalone application. All necessary files are placed to Release directory.')
parser.add_argument("Exe", help="Full path to .exe file of current Qt project")
parser.add_argument("QtBin", help="Full path to Qt binaries. It should contain such files as Qt5Core.dll, Qt5Gui.dll, etc.")
args = parser.parse_args()

dll_list = ['icudt51.dll', 'icuin51.dll',  'icuuc51.dll', 'libstdc++-6.dll', 'libwinpthread-1.dll', 'Qt5Core.dll', 'Qt5Gui.dll', 'Qt5Network.dll', 'Qt5Test.dll', 'Qt5Widgets.dll']

exe = args.Exe
qtbin_dir = args.QtBin
release_dir = os.path.join(os.getcwd(), 'Release')

if os.path.exists(release_dir):
	shutil.rmtree(release_dir)
os.mkdir(release_dir)

shutil.copy2(exe, release_dir)
for dll in dll_list:
    shutil.copy2(os.path.join(qtbin_dir, dll), release_dir)

print 'Release prepared.'