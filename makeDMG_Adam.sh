# remove & rebuild app and macdeploy it

appName="PBE"
appFile="PBE.app"
dmgFile="PBE_Mac_Download.dmg"

QTDIR="/Users/adamzs/Qt/5.15.2/clang_64/"

pathToQuaZip="/Users/adamzs/Repos/PBE/quazip/install/lib/"

pathToBackendApps="/Users/adamzs/SimCenter"
pathToOpenSees="/Users/adamzs/SimCenter/applications/OpenSees/OpenSees3.3.0/bin"
pathToDakota="/Users/adamzs/SimCenter/applications/Dakota/bin"

pathToS3hark="/Users/adamzs/Repos/PBE/s3hark"

#pathToPython="/Users/adamzs/PythonEnvR2D"

#
# source userID file containig stuff dor signing, ok if not there
#

# source userID.sh

#
# create build dir if does not exist, cd to build, conan install and then qmake
# 

rm -fr ./build/PBE.app
rm -fr ./build/*.dmg

./makeEXE.sh


cd build
cp -fr $pathToS3hark/resources ./PBE.app/Contents/MacOS

echo $PWD
#
# Check to see if the app built
#

if ! [ -x "$(command -v open $pathApp)" ]; then
	echo "$appFile did not build. Exiting."
	exit 
fi

#
# macdeployqt it
#

macdeployqt ./PBE.app 
#-qmldir=$pathToShark

#
# add missing files from macdeployqt (a known bug)
#


mkdir -p ./$appFile/Contents/plugins/renderers/
cp -R $QTDIR/plugins/renderers/libopenglrenderer.dylib ./$appFile/Contents/plugins/renderers/

echo "cp -R $QTDIR/plugins/renderplugins/libscene2d.dylib ./$appFile/Contents/plugins/renderplugins/"

mkdir -p ./$appFile/Contents/plugins/renderplugins/
cp -R $QTDIR/plugins/renderplugins/libscene2d.dylib ./$appFile/Contents/plugins/renderplugins/

# copy quazip libraries
cp $pathToQuaZip/libquazip1-qt5.1.4.0.dylib ./$appFile/Contents/Frameworks/
cp $pathToQuaZip/libquazip1-qt5.1.4.dylib ./$appFile/Contents/Frameworks/
cp $pathToQuaZip/libquazip1-qt5.dylib ./$appFile/Contents/Frameworks/

# copy applications folderm opensees and dakota
echo "cp -fR $pathToBackendApps/applications ./$appFile/Contents/MacOS"
cp -fR $pathToBackendApps/applications ./$appFile/Contents/MacOS
mkdir  ./$appFile/Contents/MacOS/applications/opensees
mkdir  ./$appFile/Contents/MacOS/applications/dakota
echo "cp -fr $pathToOpenSees/* $pathApp/Contents/MacOS/applications/opensees"
cp -fr $pathToOpenSees/* ./$appFile/Contents/MacOS/applications/opensees
cp -fr $pathToDakota/*  ./$appFile/Contents/MacOS/applications/dakota



# clean up
declare -a notWantedApp=("createBIM" 
			 "performRegionalMapping"
			 "performRegionalEventSimulation"
			 "createEDP/standardEarthquakeEDP_R"
			 "createEDP/userEDP_R"
			 "createEDP/simpleEDP"
			 "createEDP/standardWindEDP"
			 "createEVENT/ASCE7_WindSpeed"
			 "createEVENT/CFDEvent"
			 "createEVENT/hazardBasedEvent"
			 "createEVENT/DEDM_HRP"
			 "createEVENT/stochasticWind"
			 "createEVENT/stochasticWind"
			 "createEVENT/ASCE7_WindSpeed"
			 "createEVENT/LLNL_SW4"
			 "createEVENT/windTunnelExperiment"
			 "createEVENT/LowRiseTPU"
			 "createEVENT/pointWindSpeed"
			 "createSAM/MDOF-LU"
			 "performSIMULATION/IMasEDP"
			 "performSIMULATION/extractPGA"
			 "performSIMULATION/openSees_R"
			)

for app in "${notWantedApp[@]}"
do
   echo "removing $app"
   rm -fr ./$appFile/Contents/MacOS/applications/$app
done

find ./$appFile -name __pycache__ -exec rm -rf {} +;